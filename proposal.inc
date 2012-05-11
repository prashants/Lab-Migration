<?php
// $Id$

/*
	Approval Status :
	0 - Pending
	1 - Approved
	2 - Dis-Approved
	3 - Solved
  Solution Status :
	0 - Pending
	1 - Approved
	2 - Dis-Approved
	3 - Completed
  Solution Display :
  0 - No
  1 - Yes

  Tables :
  lab_migration_solution : approval_status
  	0 - Pending
  	1 - Approved
  	2 - Disapproved (delete it)
*/

function lab_migration_proposal_form($form_state)
{
  global $user;

  /************************ start approve book details ************************/
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE uid = %d ORDER BY id DESC LIMIT 1", $user->uid);
  $proposal_data = db_fetch_object($proposal_q);
  if ($proposal_data)
  {
		if ($proposal_data->approval_status == 0 || $proposal_data->approval_status == 1) {
			drupal_set_message(t('We have already received your proposal.'), 'status');
			drupal_goto('');
			return;
		}
  }

  $form['#attributes'] = array('enctype' => "multipart/form-data");

  $form['name_title'] = array(
    '#type' => 'select',
    '#title' => t('Title'),
    '#options' => array('Mr' => 'Mr', 'Ms' => 'Ms', 'Mrs' => 'Mrs', 'Dr' => 'Dr', 'Prof' => 'Prof'),
    '#required' => TRUE,
  );
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name of the Proposer'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
  );
  $form['email_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#size' => 30,
    '#value' => $user->mail,
    '#disabled' => TRUE,
  );
  $form['contact_ph'] = array(
    '#type' => 'textfield',
    '#title' => t('Contact Phone No.'),
    '#size' => 30,
    '#maxlength' => 15,
    '#required' => TRUE,
  );
  $form['department'] = array(
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
  $form['university'] = array(
    '#type' => 'textfield',
    '#title' => t('University/Institute'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
  );
  $form['lab_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the Lab'),
    '#size' => 50,
    '#required' => TRUE,
  );

  $first_experiemnt = TRUE;
  for ($counter = 1; $counter <= 15; $counter++) {
		$form['lab_experiment-' . $counter] = array(
			'#type' => 'textfield',
			'#title' => t('Title of the Experiment ') . $counter,
			'#size' => 50,
			'#required' => $first_experiemnt,
			);
			$first_experiemnt = FALSE;
	}

  $form['solution_provider_uid'] = array(
	  '#type' => 'radios',
		'#title' => t('Do you want to provide the solution'),
		'#options' => array('1' => 'Yes', '2' => 'No'),
		'#required' => TRUE,
		'#default_value' => '1',
  );

  $form['solution_display'] = array(
	  '#type' => 'radios',
		'#title' => t('Do you want to display the solution on the www.scilab.in website'),
		'#options' => array('1' => 'Yes', '2' => 'No'),
		'#required' => TRUE,
		'#default_value' => '1',
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  return $form;
}

function lab_migration_proposal_form_validate($form, &$form_state)
{
  if (!preg_match('/^[0-9\ \+]{0,15}$/', $form_state['values']['contact_ph']))
    form_set_error('mobile', t('Invalid contact phone number'));
  return;
}

function lab_migration_proposal_form_submit($form, &$form_state)
{
  global $user;

  if (!$user->uid) {
		drupal_set_message('It is mandatory to login on this website to access the proposal form', 'error');
		return;
  }

  $solution_provider_uid = 0;
  if ($form_state['values']['solution_provider_uid'] == "1")
  	$solution_provider_uid = $user->uid;
  else
  	$solution_provider_uid = 0;

  $solution_display = 0;
  if ($form_state['values']['solution_display'] == "1")
  	$solution_display = 1;
  else
  	$solution_display = 0;

  /* inserting the user proposal */
  $result = db_query("INSERT INTO {lab_migration_proposal} 
    (uid, approver_uid, name_title, name, contact_ph, department, university, lab_title, approval_status, solution_status, solution_provider_uid, solution_display, creation_date, approval_date, solution_date) VALUES
    (%d, %d, '%s', '%s', '%s', '%s', '%s', '%s', %d, %d, %d, %d, %d, 0, 0)",
    $user->uid,
    0,
    $form_state['values']['name_title'],
    $form_state['values']['name'],
    $form_state['values']['contact_ph'],
    $form_state['values']['department'],
    $form_state['values']['university'],
    $form_state['values']['lab_title'],
    0,
    0,
    $solution_provider_uid,
    $solution_display,
    time()
  );
  if (!$result)
  {
    drupal_set_message(t('Error receiving your proposal. Please try again.'), 'error');
    return;
  }
  /* proposal id */
  $proposal_id = db_last_insert_id('lab_migration_proposal', 'id');

  /* adding experiments */
  $number = 1;
  for ($counter = 1; $counter <= 15; $counter++) {
  	$experiment_field_name = 'lab_experiment-' . $counter;
  	if (strlen(trim($form_state['values'][$experiment_field_name])) >= 1) {
			$result = db_query("INSERT INTO {lab_migration_experiment} (proposal_id, number, title) VALUES (%d, %d, '%s')", $proposal_id, $number, trim($form_state['values'][$experiment_field_name]));
			if (!$result)
			{
				drupal_set_message(t('Could not insert Title of the Experiment : ') . trim($form_state['values'][$experiment_field_name]), 'error');
			} else {
				$number++;
			}
  	}
	}

  /* sending email */
  $email_to = $user->mail;
  $param['proposal_received']['proposal_id'] = $proposal_id;
  $param['proposal_received']['user_id'] = $user->uid;
  if (!drupal_mail('lab_migration', 'proposal_received', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
    drupal_set_message('Error sending email message.', 'error');

  /* sending email */
  $email_to = variable_get('lab_migration_emails', '');
  if (!drupal_mail('lab_migration', 'proposal_received', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
    drupal_set_message('Error sending email message.', 'error');

  drupal_set_message(t('We have received you Lab migration proposal. We will get back to you soon.'), 'status');
  drupal_goto('');
}

