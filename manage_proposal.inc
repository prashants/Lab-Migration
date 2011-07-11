<?php
// $Id$

function _proposal_pending()
{
  /* get pending proposals to be approved */
  $pending_rows = array();
  $pending_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE approval_status = 0 ORDER BY id DESC");
  while ($pending_data = db_fetch_object($pending_q)) {
    $pending_rows[$pending_data->id] = array(date('d-m-Y', $pending_data->creation_date), l($pending_data->name_title . ' ' . $pending_data->name, 'user/' . $pending_data->uid), $pending_data->department, $pending_data->university, $pending_data->lab_title, l('Approve', 'lab_migration/manage_proposal/approve/' . $pending_data->id) . ' | ' . l('Edit', '/lab_migration/manage_proposal/edit/' . $pending_data->id));
  }

  /* check if there are any pending proposals */
  if (!$pending_rows) {
    drupal_set_message(t('There are no pending proposals.'), 'status');
    return '';
  }

  $pending_header = array('Date of Submission', 'Name of the Proposer', 'Department/Branch', 'University/Institute', 'Title of the Lab', 'Action');
  $output = theme_table($pending_header, $pending_rows);
  return $output; 
}

function _proposal_all()
{
  /* get pending proposals to be approved */
  $proposal_rows = array();
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} ORDER BY id DESC");
  while ($proposal_data = db_fetch_object($proposal_q)) {
    $proposal_status = '';
    switch ($proposal_data->approval_status) {
    case 0: $proposal_status = 'Pending'; break;
    case 1: $proposal_status = 'Approved'; break;
    case 2: $proposal_status = 'Dis-approved'; break;
    default: $proposal_status = 'Unknown'; break;
    }
    $proposal_rows[] = array(date('d-m-Y', $proposal_data->creation_date), l($proposal_data->name_title . ' ' . $proposal_data->name, 'user/' . $proposal_data->uid), $proposal_data->department, $proposal_data->university, $proposal_data->lab_title, $proposal_status, l('Approve', 'lab_migration/manage_proposal/approve/' . $proposal_data->id) . ' | ' . l('Edit', 'lab_migration/manage_proposal/edit/' . $proposal_data->id)); 
  }

  /* check if there are any pending proposals */
  if (!$proposal_rows) {
    drupal_set_message(t('There are no proposals.'), 'status');
    return '';
  }

  $proposal_header = array('Date of Submission', 'Name of the Proposer', 'Department/Branch', 'University/Institute', 'Title of the Lab', 'Status', 'Action');
  $output = theme_table($proposal_header, $proposal_rows);
  return $output; 
}

/******************************************************************************/
/************************** PROPOSAL APPROVAL FORM ****************************/
/******************************************************************************/

function proposal_approval_form($form_state)
{
  global $user;
  $dl_root_path = 'sites/default/files/lab_migration/';

  /* get current proposal */
  $proposal_id = arg(3);
  $result = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  if (!($row = db_fetch_object($result))) {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('lab_migration/manage_proposal');
    return;
  }

  $form['name_title'] = array(
    '#type' => 'item',
    '#value' => $row->name_title,
    '#title' => t('Title'),
  );
  $form['name'] = array(
    '#type' => 'item',
    '#value' => l($row->name, 'user/' . $row->uid),
    '#title' => t('Name of the Proposer'),
  );
  $form['email'] = array(
    '#type' => 'item',
    '#value' => user_load($row->uid)->mail,
    '#title' => t('Email'),
  );
  $form['contact_ph'] = array(
    '#type' => 'item',
    '#value' => $row->contact_ph,
    '#title' => t('Contact Phone No.'),
  );
  $form['department'] = array(
    '#type' => 'item',
    '#value' => $row->department,
    '#title' => t('Department/Branch'),
  );
  $form['university'] = array(
    '#type' => 'item',
    '#value' => $row->university,
    '#title' => t('University/Institute'),
  );
  $form['lab_title'] = array(
    '#type' => 'item',
    '#value' => $row->lab_title,
    '#title' => t('Title of the Lab'),
  );
  $form['problem_topic'] = array(
    '#type' => 'item',
    '#value' => $row->problem_topic,
    '#title' => t('Topic of the Problem'),
  );

  $problem_file_q = db_query("SELECT * FROM {lab_migration_files} WHERE link_id = %d AND filetype = 'P'", $row->id);
  $problem_file_data = db_fetch_object($problem_file_q);
  if ($problem_file_data) {
		$form['problem_file'] = array(
			'#type' => 'item',
			'#value' => l($problem_file_data->filename, $dl_root_path . $problem_file_data->filepath),
			'#title' => t('Problem statement'),
		);
	}

  $sup_file_q = db_query("SELECT * FROM {lab_migration_files} WHERE link_id = %d AND filetype = 'S'", $row->id);
  $sup_file_data = db_fetch_object($sup_file_q);
  if ($sup_file_data) {
		$form['supplementary_file'] = array(
			'#type' => 'item',
			'#value' => l($sup_file_data->filename, $dl_root_path . $sup_file_data->filepath),
			'#title' => t('Supplementary file'),
		);
	}

	if ($row->approval_status > 0) {
		$form['approval'] = array(
			'#type' => 'radios',
			'#title' => t('Lab migration proposal'),
			'#options' => array('1' => 'Approve', '2' => 'Disapprove'),
			'#required' => TRUE,
			'#default_value' => $row->approval_status,
		);
		$approver_user_data = user_load($row->approver_uid );
		$form['approval_details'] = array(
			'#type' => 'item',
			'#value' => 'By : ' . l($approver_user_data->name, 'user/' . $row->approver_uid) . '<br />'
									. 'On : ' . date('d-m-Y', $row->approval_date),
		);
	} else {
		$form['approval'] = array(
			'#type' => 'radios',
			'#title' => t('Lab migration proposal'),
			'#options' => array('1' => 'Approve', '2' => 'Disapprove'),
			'#required' => TRUE,
		);
	}

  $form['message'] = array(
    '#type' => 'textarea',
    '#title' => t('Reason for disapproval'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );

  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'lab_migration/manage_proposal'),
  );

  return $form;
}

function proposal_approval_form_submit($form, &$form_state)
{
  global $user;

  /* get current proposal */
  $proposal_id = (int)arg(3);
  $result = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  if (!($row = db_fetch_object($result))) {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('lab_migration/manage_proposal');
    return;
  }

  if ($form_state['values']['approval'] == 1) {
  	db_query("UPDATE {lab_migration_proposal} SET approver_uid = %d, approval_date = %d, approval_status = 1 WHERE id = %d", $user->uid, time(), $proposal_id);

    /* sending email */
    $user_data = user_load($row->uid);
    $email_to = $user_data->mail;
    $param['proposal_approved']['proposal_id'] = $proposal_id;
    $param['proposal_approved']['user_id'] = $row->uid;
    if (!drupal_mail('lab_migration', 'proposal_approved', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
      drupal_set_message('Error sending email message.', 'error');

    $email_to = $user->mail . ', ' . variable_get('textbook_companion_emails', '');;
    if (!drupal_mail('lab_migration', 'proposal_approved', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
      drupal_set_message('Error sending email message.', 'error');

    drupal_set_message('Lab migration proposal No. ' . $proposal_id  . ' approved. User has been notified of the approval.', 'status');
    drupal_goto('lab_migration/manage_proposal');
    return;
  } else if ($form_state['values']['approval'] == 2) {
    db_query("UPDATE {lab_migration_proposal} SET approver_uid = %d, approval_date = %d, approval_status = 2, message = '%s' WHERE id = %d", $user->uid, time(), $form_state['values']['message'], $proposal_id);

    /* sending email */
    $user_data = user_load($row->uid);
    $email_to = $user_data->mail;
    $param['proposal_disapproved']['proposal_id'] = $proposal_id;
    $param['proposal_disapproved']['user_id'] = $row->uid;
    if (!drupal_mail('lab_migration', 'proposal_disapproved', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
      drupal_set_message('Error sending email message.', 'error');

    $email_to = $user->mail . ', ' . variable_get('textbook_companion_emails', '');;
    if (!drupal_mail('lab_migration', 'proposal_disapproved', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
      drupal_set_message('Error sending email message.', 'error');

    drupal_set_message('Lab migration proposal No. ' . $proposal_id  . ' dis-approved. User has been notified of the dis-approval.', 'error');
    drupal_goto('lab_migration/manage_proposal');
    return;
  }
}

/******************************************************************************/
/**************************** PROPOSAL EDIT FORM ******************************/
/******************************************************************************/

function proposal_edit_form($form_state)
{
  global $user;
  $dl_root_path = 'sites/default/files/lab_migration/';

  /* get current proposal */
  $proposal_id = arg(3);
  $result = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  if (!($row = db_fetch_object($result))) {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('lab_migration/manage_proposal');
    return;
  }

  $user_data = user_load($proposal_data->uid);

  $form['name_title'] = array(
    '#type' => 'select',
    '#title' => t('Title'),
    '#options' => array('Mr' => 'Mr', 'Ms' => 'Ms', 'Mrs' => 'Mrs', 'Dr' => 'Dr', 'Prof' => 'Prof'),
    '#required' => TRUE,
    '#default_value' => $row->name_title,
  );
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name of the Proposer'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $row->name,
  );
  $form['email_id'] = array(
    '#type' => 'item',
    '#title' => t('Email'),
    '#value' => $user->mail,
  );
  $form['contact_ph'] = array(
    '#type' => 'textfield',
    '#title' => t('Contact Phone No.'),
    '#size' => 30,
    '#maxlength' => 15,
    '#required' => TRUE,
    '#default_value' => $row->contact_ph,
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
    '#default_value' => $row->department,
  );
  $form['university'] = array(
    '#type' => 'textfield',
    '#title' => t('University/Institute'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $row->university,
  );
  $form['lab_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the Lab'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $row->lab_title,
  );
  $form['problem_topic'] = array(
    '#type' => 'textfield',
    '#title' => t('Topic of the Problem'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $row->problem_topic,
  );

  $problem_file_q = db_query("SELECT * FROM {lab_migration_files} WHERE link_id = %d AND filetype = 'P'", $row->id);
  $problem_file_data = db_fetch_object($problem_file_q);
  if ($problem_file_data) {
		$form['problem_file'] = array(
			'#type' => 'item',
			'#value' => l($problem_file_data->filename, $dl_root_path . $problem_file_data->filepath),
			'#title' => t('Problem statement'),
		);
	}

  $sup_file_q = db_query("SELECT * FROM {lab_migration_files} WHERE link_id = %d AND filetype = 'S'", $row->id);
  $sup_file_data = db_fetch_object($sup_file_q);
  if ($sup_file_data) {
		$form['supplementary_file'] = array(
			'#type' => 'item',
			'#value' => l($sup_file_data->filename, $dl_root_path . $sup_file_data->filepath),
			'#title' => t('Supplementary file'),
		);
	}

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'lab_migration/manage_proposal'),
  );
  return $form;
}

function proposal_edit_form_validate($form, &$form_state)
{
  /* contact phone */
  if (!preg_match('/^[0-9\ \+]{0,15}$/', $form_state['values']['mobile']))
    form_set_error('mobile', t('Invalid mobile number'));
  return;
}

function proposal_edit_form_submit($form, &$form_state)
{
  /* get current proposal */
  $proposal_id = arg(3);
  $result = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  if (!($row = db_fetch_object($result))) {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('lab_migration/manage_proposal');
    return;
  }

  db_query("UPDATE {lab_migration_proposal} SET name_title = '%s', name = '%s', contact_ph = '%s', department = '%s', university = '%s', lab_title = '%s', problem_topic = '%s' WHERE id = %d",
  	$form_state['values']['name_title'],
    $form_state['values']['name'],
    $form_state['values']['contact_ph'],
    $form_state['values']['department'],
    $form_state['values']['university'],
    $form_state['values']['lab_title'],
    $form_state['values']['problem_topic'],
    $row->id
  );
  drupal_set_message(t('Proposal Updated'), 'status');
  drupal_goto('lab_migration/manage_proposal');
}

