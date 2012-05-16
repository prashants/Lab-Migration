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

function lab_migration_proposal_display($form_state)
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
    '#title' => t('Name'),
  );
  $form['email_id'] = array(
    '#type' => 'item',
    '#value' => user_load($proposal_data->uid)->mail,
    '#title' => t('Email'),
  );
  $form['contact_ph'] = array(
    '#type' => 'item',
    '#value' => $proposal_data->contact_ph,
    '#title' => t('Contact Phone No.'),
  );
  $form['department'] = array(
    '#type' => 'item',
    '#value' => $proposal_data->department,
    '#title' => t('Department/Branch'),
  );
  $form['university'] = array(
    '#type' => 'item',
    '#value' => $proposal_data->university,
    '#title' => t('University/Institute'),
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

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Apply for Solution')
  );
  return $form;
}

function lab_migration_proposal_display_submit($form_state, &$form_state)
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

	$solution_provider_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE solution_provider_uid = %d AND approval_status IN (0, 1, 2) AND solution_status IN (0, 1)", $user->uid);
	if (db_fetch_object($solution_provider_q)) {
    drupal_set_message("You have already applied for a solution. Please compelete that before applying for another solution.", 'error');
    drupal_goto('lab_migration/open_proposal');
	}

  db_query("UPDATE {lab_migration_proposal} set solution_provider_uid = %d, solution_status = 0 WHERE id = %d", $user->uid, $proposal_id);
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

