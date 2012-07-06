<?php
// $Id$

function lab_migration_proposal_open()
{
  global $user;

  /* get open proposal list */
  $proposal_rows = array();
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE approval_status = 1 AND solution_provider_uid = 0");
  while ($proposal_data = db_fetch_object($proposal_q))
  {
    $proposal_rows[] = array(l($proposal_data->lab_title, 'lab_migration/show_proposal/' . $proposal_data->id), l('Apply', 'lab_migration/show_proposal/' . $proposal_data->id));
  }

  $proposal_header = array('Title of the Lab', 'Actions');
  $return_html .= theme_table($proposal_header, $proposal_rows);
  return $return_html;
}

function lab_migration_solution_proposal_form($form_state)
{
  global $user;

  $proposal_id = (int)arg(2);

  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  $proposal_data = db_fetch_object($proposal_q);
  if (!$proposal_data)
  {
    drupal_set_message("Invalid proposal.", 'error');
    drupal_goto('');
  }

  $form['name'] = array(
    '#type' => 'item',
    '#value' => l($proposal_data->name_title . ' ' . $proposal_data->name, 'user/' . $proposal_data->uid),
    '#title' => t('Proposer Name'),
  );
  $form['lab_title'] = array(
    '#type' => 'item',
    '#value' => $proposal_data->lab_title,
    '#title' => t('Title of the Lab'),
  );

  $experiment_html = '';
  $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d", $proposal_id);
  while ($experiment_data = db_fetch_object($experiment_q)) {
 		$experiment_html .= $experiment_data->title . "<br/>";
	}
  $form['experiment'] = array(
    '#type' => 'item',
    '#value' => $experiment_html,
    '#title' => t('Experiment List'),
  );

  $form['solution_provider_name_title'] = array(
    '#type' => 'select',
    '#title' => t('Title'),
    '#options' => array('Mr' => 'Mr', 'Ms' => 'Ms', 'Mrs' => 'Mrs', 'Dr' => 'Dr', 'Prof' => 'Prof'),
    '#required' => TRUE,
  );
  $form['solution_provider_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name of the Solution Provider'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
  );
  $form['solution_provider_email_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#size' => 30,
    '#value' => $user->mail,
    '#disabled' => TRUE,
  );
  $form['solution_provider_contact_ph'] = array(
    '#type' => 'textfield',
    '#title' => t('Contact No.'),
    '#size' => 30,
    '#maxlength' => 15,
    '#required' => TRUE,
  );
  $form['solution_provider_department'] = array(
    '#type' => 'select',
    '#title' => t('Department/Branch'),
    '#options' => array('' => 'Please select...',
                        'Computer Engineering' => 'Computer Engineering',
    										'Electrical Engineering' => 'Electrical Engineering',
                        'Electronics Engineering' => 'Electronics Engineering',
                        'Chemical Engineering' => 'Chemical Engineering',
                        'Instrumentation Engineering' => 'Instrumentation Engineering',
                        'Mechanical Engineering' => 'Mechanical Engineering',
                        'Civil Engineering' => 'Civil Engineering',
                        'Physics' => 'Physics',
                        'Mathematics' => 'Mathematics',
                        'Others' => 'Others'),
    '#required' => TRUE,
  );
  $form['solution_provider_university'] = array(
    '#type' => 'textfield',
    '#title' => t('University/Institute'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
  );

  $form['solution_provider_expected_completion_date'] = array(
    '#type' => 'textfield',
    '#title' => t('Expected Completion Date'),
    '#description' => t('Input date format should be DD-MM-YYYY. Eg: 23-03-2011'),
    '#size' => 10,
    '#maxlength' => 10,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Apply for Solution')
  );
  return $form;
}

function lab_migration_solution_proposal_form_validate($form_state, &$form_state)
{
	global $user;

	$solution_provider_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE solution_provider_uid = %d AND approval_status IN (0, 1) AND solution_status IN (0, 1, 2)", $user->uid);
	if (db_fetch_object($solution_provider_q)) {
    form_set_error('', t("You have already applied for a solution. Please compelete that before applying for another solution."));
    drupal_goto('lab_migration/open_proposal');
	}

  /* date of completion */
  if (!preg_match('/^[0-9]{1,2}-[0-9]{1,2}-[0-9]{4}$/', $form_state['values']['solution_provider_expected_completion_date']))
    form_set_error('solution_provider_expected_completion_date', t('Invalid expected date of completion'));

  list($d, $m, $y) = explode('-', $form_state['values']['solution_provider_expected_completion_date']);
  $d = (int)$d; $m = (int)$m; $y = (int)$y;
  if (!checkdate($m, $d, $y))
    form_set_error('solution_provider_expected_completion_date', t('Invalid expected date of completion'));
  if (mktime(0, 0, 0, $m, $d, $y) <= time())
   form_set_error('solution_provider_expected_completion_date', t('Expected date of completion should be in future')); 
}

function lab_migration_solution_proposal_form_submit($form_state, &$form_state)
{
  global $user;

  $proposal_id = (int)arg(2);

  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  $proposal_data = db_fetch_object($proposal_q);
  if (!$proposal_data)
  {
    drupal_set_message("Invalid proposal.", 'error');
    drupal_goto('lab_migration/open_proposal');
  }
  if ($proposal_data->solution_provider_uid != 0) {
    drupal_set_message("Someone has already applied for solving this Lab.", 'error');
    drupal_goto('lab_migration/open_proposal');
  }

  db_query("UPDATE {lab_migration_proposal} set solution_provider_uid = %d, solution_status = 1, solution_provider_name_title = '%s', solution_provider_name = '%s', solution_provider_contact_ph = '%s', solution_provider_department = '%s', solution_provider_university = '%s', expected_completion_date = '%s' WHERE id = %d",
    $user->uid,
    $form_state['values']['solution_provider_name_title'],
    $form_state['values']['solution_provider_name'],
    $form_state['values']['solution_provider_contact_ph'],
    $form_state['values']['solution_provider_department'],
    $form_state['values']['solution_provider_university'],
    $form_state['values']['solution_provider_expected_completion_date'],
    $proposal_id);
	drupal_set_message("We have received your application. We will get back to you soon.", 'status');

  /* sending email */
  $email_to = $user->mail;
  $param['solution_proposal_received']['proposal_id'] = $proposal_id;
  $param['solution_proposal_received']['user_id'] = $user->uid;
  if (!drupal_mail('lab_migration', 'solution_proposal_received', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
    drupal_set_message('Error sending email message.', 'error');

  /* sending email */
  $email_to = variable_get('lab_migration_emails', '');
  if (!drupal_mail('lab_migration', 'solution_proposal_received', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
    drupal_set_message('Error sending email message.', 'error');

	drupal_goto('lab_migration/open_proposal');
}

