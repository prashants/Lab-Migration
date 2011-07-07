<?php
// $Id$

function _proposal_pending()
{
  /* get pending proposals to be approved */
  $pending_rows = array();
  $pending_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE approval_status = 0 ORDER BY id DESC");
  while ($pending_data = db_fetch_object($pending_q))
  {
    $pending_rows[$pending_data->id] = array(date('d-m-Y', $pending_data->creation_date), l($pending_data->name, 'user/' . $pending_data->uid), $pending_data->department, $pending_data->university, $pending_data->lab_title, l('Approve', 'lab_migration/manage_proposal/approve/' . $pending_data->id) . ' | ' . l('Edit', 'manage_proposal/edit/' . $pending_data->id));
  }

  /* check if there are any pending proposals */
  if (!$pending_rows)
  {
    drupal_set_message(t('There are no pending proposals.'), 'status');
    return '';
  }

  $pending_header = array('Date of Submission', 'Name of the Professor', 'Department/Branch', 'University/Institute', 'Title of the Lab', 'Action');
  $output = theme_table($pending_header, $pending_rows);
  return $output; 
}

function _proposal_all()
{
  /* get pending proposals to be approved */
  $proposal_rows = array();
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} ORDER BY id DESC");
  while ($proposal_data = db_fetch_object($proposal_q))
  {
    $proposal_status = '';
    switch ($proposal_data->approval_status)
    {
    case 0: $proposal_status = 'Pending'; break;
    case 1: $proposal_status = 'Approved'; break;
    case 2: $proposal_status = 'Dis-approved'; break;
    case 3: $proposal_status = 'Completed'; break;
    default: $proposal_status = 'Unknown'; break;
    }
    $proposal_rows[] = array(date('d-m-Y', $proposal_data->creation_date), l($proposal_data->name, 'user/' . $proposal_data->uid), $proposal_data->department, $proposal_data->university, $proposal_data->lab_title, $proposal_status, l('Approve', 'lab_migration/manage_proposal/approve/' . $proposal_data->id) . ' | ' . l('Edit', 'manage_proposal/edit/' . $proposal_data->id)); 
  }

  /* check if there are any pending proposals */
  if (!$proposal_rows)
  {
    drupal_set_message(t('There are no proposals.'), 'status');
    return '';
  }

  $proposal_header = array('Date of Submission', 'Name of the Professor', 'Department/Branch', 'University/Institute', 'Title of the Lab', 'Status', 'Action');
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

  $form['name'] = array(
    '#type' => 'item',
    '#value' => l($row->name, 'user/' . $row->uid),
    '#title' => t('Name of the Professor'),
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

  /* get current proposal */
  $proposal_id = arg(2);
  $proposal_q = db_query("SELECT * FROM {textbook_companion_proposal} WHERE id = %d", $proposal_id);
  if ($proposal_q)
  {
    $proposal_data = db_fetch_object($proposal_q);
    if (!$proposal_data)
    {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('manage_proposal');
      return;
    }
  } else {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('manage_proposal');
    return;
  }

  $user_data = user_load($proposal_data->uid);

  $preference1_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $proposal_id, 1);
  $preference1_data = db_fetch_object($preference1_q);
  $preference2_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $proposal_id, 2);
  $preference2_data = db_fetch_object($preference2_q);
  $preference3_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $proposal_id, 3);
  $preference3_data = db_fetch_object($preference3_q);

  $form['full_name'] = array(
    '#type' => 'textfield',
    '#title' => t('Full Name'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $proposal_data->full_name,
  );
  $form['email_id'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#size' => 30,
    '#value' => $user_data->mail,
    '#disabled' => TRUE,
  );
  $form['mobile'] = array(
    '#type' => 'textfield',
    '#title' => t('Mobile No.'),
    '#size' => 30,
    '#maxlength' => 15,
    '#required' => TRUE,
    '#default_value' => $proposal_data->mobile,
  );
  $form['how_project'] = array(
    '#type' => 'select',
    '#title' => t('How did you come to know about this project'),
    '#options' => array('Scilab Website' => 'Scilab Website',
                        'Friend' => 'Friend',
                        'Professor/Teacher' => 'Professor/Teacher',
                        'Mailing List' => 'Mailing List',
                        'Poster in my/other college' => 'Poster in my/other college',
                        'Others' => 'Others'),
    '#required' => TRUE,
    '#default_value' => $proposal_data->how_project,
  );
  $form['course'] = array(
    '#type' => 'textfield',
    '#title' => t('Course'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $proposal_data->course,
  );
  $form['branch'] = array(
    '#type' => 'select',
    '#title' => t('Department/Branch'),
    '#options' => array('Electrical Engineering' => 'Electrical Engineering',
                        'Electronics Engineering' => 'Electronics Engineering',
                        'Computer Engineering' => 'Computer Engineering',
                        'Chemical Engineering' => 'Chemical Engineering',
                        'Instrumentation Engineering' => 'Instrumentation Engineering',
                        'Mechanical Engineering' => 'Mechanical Engineering',
                        'Civil Engineering' => 'Civil Engineering',
                        'Physics' => 'Physics',
                        'Mathematics' => 'Mathematics',
                        'Others' => 'Others'),
    '#required' => TRUE,
    '#default_value' => $proposal_data->branch,
  );
  $form['university'] = array(
    '#type' => 'textfield',
    '#title' => t('University/Institute'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $proposal_data->university,
  );
  $form['faculty'] = array(
    '#type' => 'textfield',
    '#title' => t('College Teacher/Professor'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $proposal_data->faculty,
  );
  $form['reviewer'] = array(
    '#type' => 'textfield',
    '#title' => t('Reviewer'),
    '#size' => 30,
    '#maxlength' => 100,
    '#default_value' => $proposal_data->reviewer,
  );
  $form['completion_date'] = array(
    '#type' => 'textfield',
    '#title' => t('Expected Date of Completion'),
    '#description' => t('Input date format should be DD-MM-YYYY. Eg: 23-03-2011'),
    '#size' => 10,
    '#maxlength' => 10,
    '#default_value' => date('d-m-Y', $proposal_data->completion_date),
  );
  $form['preference1'] = array(
    '#type' => 'fieldset',
    '#title' => t('Book Preference 1'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['preference1']['book1'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the book'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $preference1_data->book,
  );
  $form['preference1']['author1'] = array(
    '#type' => 'textfield',
    '#title' => t('Author Name'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $preference1_data->author,
  );
  $form['preference1']['isbn1'] = array(
    '#type' => 'textfield',
    '#title' => t('ISBN No'),
    '#size' => 30,
    '#maxlength' => 25,
    '#required' => TRUE,
    '#default_value' => $preference1_data->isbn,
  );
  $form['preference1']['publisher1'] = array(
    '#type' => 'textfield',
    '#title' => t('Publisher & Place'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $preference1_data->publisher,
  );
  $form['preference1']['edition1'] = array(
    '#type' => 'textfield',
    '#title' => t('Edition'),
    '#size' => 4,
    '#maxlength' => 2,
    '#required' => TRUE,
    '#default_value' => $preference1_data->edition,
  );
  $form['preference1']['year1'] = array(
    '#type' => 'textfield',
    '#title' => t('Year of pulication'),
    '#size' => 4,
    '#maxlength' => 4,
    '#required' => TRUE,
    '#default_value' => $preference1_data->year,
  );
  $form['preference2'] = array(
    '#type' => 'fieldset',
    '#title' => t('Book Preference 2'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['preference2']['book2'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the book'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $preference2_data->book,
  );
  $form['preference2']['author2'] = array(
    '#type' => 'textfield',
    '#title' => t('Author Name'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $preference2_data->author,
  );
  $form['preference2']['isbn2'] = array(
    '#type' => 'textfield',
    '#title' => t('ISBN No'),
    '#size' => 30,
    '#maxlength' => 25,
    '#required' => TRUE,
    '#default_value' => $preference2_data->isbn,
  );
  $form['preference2']['publisher2'] = array(
    '#type' => 'textfield',
    '#title' => t('Publisher & Place'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $preference2_data->publisher,
  );
  $form['preference2']['edition2'] = array(
    '#type' => 'textfield',
    '#title' => t('Edition'),
    '#size' => 4,
    '#maxlength' => 2,
    '#required' => TRUE,
    '#default_value' => $preference2_data->edition,
  );
  $form['preference2']['year2'] = array(
    '#type' => 'textfield',
    '#title' => t('Year of pulication'),
    '#size' => 4,
    '#maxlength' => 4,
    '#required' => TRUE,
    '#default_value' => $preference2_data->year,
  );
  $form['preference3'] = array(
    '#type' => 'fieldset',
    '#title' => t('Book Preference 3'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['preference3']['book3'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the book'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $preference3_data->book,
  );
  $form['preference3']['author3'] = array(
    '#type' => 'textfield',
    '#title' => t('Author Name'),
    '#size' => 30,
    '#maxlength' => 100,
    '#required' => TRUE,
    '#default_value' => $preference3_data->author,
  );
  $form['preference3']['isbn3'] = array(
    '#type' => 'textfield',
    '#title' => t('ISBN No'),
    '#size' => 30,
    '#maxlength' => 25,
    '#required' => TRUE,
    '#default_value' => $preference3_data->isbn,
  );
  $form['preference3']['publisher3'] = array(
    '#type' => 'textfield',
    '#title' => t('Publisher & Place'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $preference3_data->publisher,
  );
  $form['preference3']['edition3'] = array(
    '#type' => 'textfield',
    '#title' => t('Edition'),
    '#size' => 4,
    '#maxlength' => 2,
    '#required' => TRUE,
    '#default_value' => $preference3_data->edition,
  );
  $form['preference3']['year3'] = array(
    '#type' => 'textfield',
    '#title' => t('Year of pulication'),
    '#size' => 4,
    '#maxlength' => 4,
    '#required' => TRUE,
    '#default_value' => $preference3_data->year,
  );

  /* hidden fields */
  $form['hidden_proposal_id'] = array(
    '#type' => 'hidden',
    '#value' => $proposal_id,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'manage_proposal'),
  );
  return $form;
}

function proposal_edit_form_validate($form, &$form_state)
{
  /* mobile */
  if (!preg_match('/^[0-9\ \+]{0,15}$/', $form_state['values']['mobile']))
    form_set_error('mobile', t('Invalid mobile number'));

  /* date of completion */
  if (!preg_match('/^[0-9]{1,2}-[0-9]{1,2}-[0-9]{4}$/', $form_state['values']['completion_date']))
    form_set_error('completion_date', t('Invalid expected date of completion'));

  list($d, $m, $y) = explode('-', $form_state['values']['completion_date']);
  $d = (int)$d; $m = (int)$m; $y = (int)$y;
  if (!checkdate($m, $d, $y))
    form_set_error('completion_date', t('Invalid expected date of completion'));
  if (mktime(0, 0, 0, $m, $d, $y) <= time())
  	form_set_error('completion_date', t('Expected date of completion should be in future'));  

  /* edition */
  if (!preg_match('/^[1-9][0-9]{0,1}$/', $form_state['values']['edition1']))
    form_set_error('edition1', t('Invalid edition for Book Preference 1'));
  if (!preg_match('/^[1-9][0-9]{0,1}$/', $form_state['values']['edition2']))
    form_set_error('edition2', t('Invalid edition for Book Preference 2'));
  if (!preg_match('/^[1-9][0-9]{0,1}$/', $form_state['values']['edition3']))
    form_set_error('edition3', t('Invalid edition for Book Preference 3'));

  /* year of publication */
  if (!preg_match('/^[1-3][0-9][0-9][0-9]$/', $form_state['values']['year1']))
    form_set_error('year1', t('Invalid year of pulication for Book Preference 1'));
  if (!preg_match('/^[1-3][0-9][0-9][0-9]$/', $form_state['values']['year2']))
    form_set_error('year2', t('Invalid year of pulication for Book Preference 2'));
  if (!preg_match('/^[1-3][0-9][0-9][0-9]$/', $form_state['values']['year3']))
    form_set_error('year3', t('Invalid year of pulication for Book Preference 3'));

  /* year of publication */
  $cur_year = date('Y');
  if ((int)$form_state['values']['year1'] > $cur_year)
    form_set_error('year1', t('Year of pulication should be not in the future for Book Preference 1'));
  if ((int)$form_state['values']['year2'] > $cur_year)
    form_set_error('year2', t('Year of pulication should be not in the future for Book Preference 2'));
  if ((int)$form_state['values']['year3'] > $cur_year)
    form_set_error('year3', t('Year of pulication should be not in the future for Book Preference 3'));
  
  /* isbn */
  if (!preg_match('/^[0-9\-xX]+$/', $form_state['values']['isbn1']))
    form_set_error('isbn1', t('Invalid ISBN for Book Preference 1'));
  if (!preg_match('/^[0-9\-xX]+$/', $form_state['values']['isbn2']))
    form_set_error('isbn2', t('Invalid ISBN for Book Preference 2'));
  if (!preg_match('/^[0-9\-xX]+$/', $form_state['values']['isbn3']))
    form_set_error('isbn3', t('Invalid ISBN for Book Preference 3'));
  return;
}

function proposal_edit_form_submit($form, &$form_state)
{
  /* completion date to timestamp */
  list($d, $m, $y) = explode('-', $form_state['values']['completion_date']);
  $completion_date_timestamp = mktime(0, 0, 0, $m, $d, $y);

  $proposal_id = $form_state['values']['hidden_proposal_id'];

  db_query("UPDATE {textbook_companion_proposal} SET full_name = '%s', mobile = '%s', how_project = '%s', course = '%s', branch = '%s', university = '%s', faculty = '%s', reviewer = '%s', completion_date = %d WHERE id = %d",
    $form_state['values']['full_name'],
    $form_state['values']['mobile'],
    $form_state['values']['how_project'],
    $form_state['values']['course'],
    $form_state['values']['branch'],
    $form_state['values']['university'],
    $form_state['values']['faculty'],
    $form_state['values']['reviewer'],
    $completion_date_timestamp,
    $proposal_id);

  $preference1_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $proposal_id, 1);
  $preference1_data = db_fetch_object($preference1_q);
  if ($preference1_data)
  	$preference1_id = $preference1_data->id;
  $preference2_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $proposal_id, 2);
  $preference2_data = db_fetch_object($preference2_q);
  if ($preference2_data)
  	$preference2_id = $preference2_data->id;
  $preference3_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE proposal_id = %d AND pref_number = %d LIMIT 1", $proposal_id, 3);
  $preference3_data = db_fetch_object($preference3_q);
  if ($preference3_data)
  	$preference3_id = $preference3_data->id;

  if ($preference1_data)
  {
		db_query("UPDATE {textbook_companion_preference} SET book = '%s', author = '%s', isbn = '%s', publisher = '%s', edition = %d, year = %d WHERE id = %d",
			$form_state['values']['book1'],
			$form_state['values']['author1'],
			$form_state['values']['isbn1'],
			$form_state['values']['publisher1'],
			$form_state['values']['edition1'],
			$form_state['values']['year1'],
			$preference1_id);
	}
  if ($preference2_data)
  {
		db_query("UPDATE {textbook_companion_preference} SET book = '%s', author = '%s', isbn = '%s', publisher = '%s', edition = %d, year = %d WHERE id = %d",
			$form_state['values']['book2'],
			$form_state['values']['author2'],
			$form_state['values']['isbn2'],
			$form_state['values']['publisher2'],
			$form_state['values']['edition2'],
			$form_state['values']['year2'],
			$preference2_id);
	}
  if ($preference3_data)
  {
		db_query("UPDATE {textbook_companion_preference} SET book = '%s', author = '%s', isbn = '%s', publisher = '%s', edition = %d, year = %d WHERE id = %d",
			$form_state['values']['book3'],
			$form_state['values']['author3'],
			$form_state['values']['isbn3'],
			$form_state['values']['publisher3'],
			$form_state['values']['edition3'],
			$form_state['values']['year3'],
			$preference3_id);
	}
  drupal_set_message(t('Proposal Updated'), 'status');
}



/******************************************************************************/
/**************************** CATEGORY EDIT FORM ******************************/
/******************************************************************************/

function category_edit_form($form_state)
{
  /* get current proposal */
  $preference_id = arg(3);
  $preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE id = %d", $preference_id);
  $preference_data = db_fetch_object($preference_q);
	if (!$preference_data)
	{
		drupal_set_message(t('Invalid book selected. Please try again.'), 'error');
		drupal_goto('manage_proposal/category');
		return;
	}

  $form['book'] = array(
    '#type' => 'item',
    '#title' => t('Title of the book'),
    '#value' => $preference_data->book,
  );
  $form['author'] = array(
    '#type' => 'item',
    '#title' => t('Author Name'),
    '#value' => $preference_data->author,
  );
  $form['isbn'] = array(
    '#type' => 'item',
    '#title' => t('ISBN No'),
    '#value' => $preference_data->isbn,
  );
  $form['publisher'] = array(
    '#type' => 'item',
    '#title' => t('Publisher & Place'),
    '#value' => $preference_data->publisher,
  );
  $form['edition'] = array(
    '#type' => 'item',
    '#title' => t('Edition'),
    '#value' => $preference_data->edition,
  );
  $form['year'] = array(
    '#type' => 'item',
    '#title' => t('Year of pulication'),
    '#value' => $preference_data->year,
  );

  $form['category'] = array(
    '#type' => 'select',
    '#title' => t('Category'),
    '#options' => array(0 => 'Please select',
                        1 => 'Computer Science and Information Technology',
    										2 => 'Electrical and Electronics Engineering',
                        3 => 'Mechanical Engineering',
                        4 => 'Chemical Engineering',
                        5 => 'Civil Engineering',
                        6 => 'Physics',
                        7 => 'Mathematics',
                        8 => 'Others'),
    '#required' => TRUE,
    '#default_value' => $preference_data->category,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'manage_proposal/category'),
  );
  return $form;
}

function category_edit_form_submit($form, &$form_state)
{
  /* get current proposal */
  $preference_id = (int)arg(3);
  $preference_q = db_query("SELECT * FROM {textbook_companion_preference} WHERE id = %d", $preference_id);
  $preference_data = db_fetch_object($preference_q);
	if (!$preference_data)
	{
		drupal_set_message(t('Invalid book selected. Please try again.'), 'error');
		drupal_goto('manage_proposal/category');
		return;
	}

  db_query("UPDATE {textbook_companion_preference} SET category = %d WHERE id = %d", $form_state['values']['category'], $preference_data->id);

  drupal_set_message(t('Book Category Updated'), 'status');
  drupal_goto('manage_proposal/category');
}

