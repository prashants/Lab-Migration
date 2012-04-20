<?php
// $Id$

function _proposal_pending()
{
  /* get pending proposals to be approved */
  $pending_rows = array();
  $pending_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE approval_status = 0 ORDER BY id DESC");
  while ($pending_data = db_fetch_object($pending_q))
  {
    $pending_rows[$pending_data->id] = array(date('d-m-Y', $pending_data->creation_date), l($pending_data->name, 'user/' . $pending_data->uid), $pending_data->lab_title, $pending_data->department, l('Approve', 'lab_migration/manage_proposal/approve/' . $pending_data->id) . ' | ' . l('Edit', 'lab_migration/manage_proposal/edit/' . $pending_data->id));
  }

  /* check if there are any pending proposals */
  if (!$pending_rows)
  {
    drupal_set_message(t('There are no pending proposals.'), 'status');
    return '';
  }

  $pending_header = array('Date of Submission', 'Name', 'Title of the Lab', 'Department', 'Action');
  $output = theme_table($pending_header, $pending_rows);
  return $output; 
}

function _proposal_pending_solution()
{
  /* get pending proposals to be approved */
  $pending_rows = array();
  $pending_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE approval_status = 1 ORDER BY id DESC");
  while ($pending_data = db_fetch_object($pending_q))
  {
    $pending_rows[$pending_data->id] = array(date('d-m-Y', $pending_data->creation_date), l($pending_data->name, 'user/' . $pending_data->uid), $pending_data->lab_title, $pending_data->department, l('Status', 'lab_migration/manage_proposal/status/' . $pending_data->id));
  }

  /* check if there are any pending proposals */
  if (!$pending_rows)
  {
    drupal_set_message(t('There are no proposals pending for solutions.'), 'status');
    return '';
  }

  $pending_header = array('Date of Submission', 'Name', 'Title of the Lab', 'Department', 'Action');
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
    $approval_status = '';
    switch ($proposal_data->approval_status)
    {
    case 0: $approval_status = 'Pending'; break;
    case 1: $approval_status = 'Approved'; break;
    case 2: $approval_status = 'Dis-approved'; break;
    case 3: $approval_status = 'Solved'; break;
    default: $approval_status = 'Unknown'; break;
    }
    $proposal_rows[] = array(date('d-m-Y', $proposal_data->creation_date), l($proposal_data->name, 'user/' . $proposal_data->uid), $proposal_data->lab_title, $proposal_data->department, $approval_status, l('Status', 'lab_migration/manage_proposal/status/' . $proposal_data->id) . ' | ' . l('Edit', 'lab_migration/manage_proposal/edit/' . $proposal_data->id));
  }

  /* check if there are any pending proposals */
  if (!$proposal_rows)
  {
    drupal_set_message(t('There are no proposals.'), 'status');
    return '';
  }

  $proposal_header = array('Date of Submission', 'Name', 'Title of the Lab', 'Department', 'Status');
  $output = theme_table($proposal_header, $proposal_rows);
  return $output; 
}

function _category_all()
{
  /* get pending proposals to be approved */
  $proposal_rows = array();
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} ORDER BY id DESC");
  while ($proposal_data = db_fetch_object($proposal_q))
  {
  	switch ($proposal_data->category)
  	{
    case 0: $category_data = 'Not Selected'; break;
    case 1: $category_data = 'Fluid Mechanics'; break;
    case 2: $category_data = 'Control Theory & Control Systems'; break;
    case 3: $category_data = 'Chemical Engineering'; break;
    case 4: $category_data = 'Thermodynamics'; break;
    case 5: $category_data = 'Mechanical Engineering'; break;
    case 6: $category_data = 'Signal Processing'; break;
    case 7: $category_data = 'Digital Communications'; break;
    case 8: $category_data = 'Electrical Technology'; break;
    case 9: $category_data = 'Mathematics & Pure Science'; break;
    case 10: $category_data = 'Analog Electronics'; break;
    case 11: $category_data = 'Digital Electronics'; break;
    case 12: $category_data = 'Computer Programming'; break;
    case 13: $category_data = 'Others'; break;
    default: $category_data = 'Unknown'; break;
  	}
    $proposal_rows[] = array(date('d-m-Y', $proposal_data->creation_date), l($proposal_data->name, 'user/' . $proposal_data->uid), $proposal_data->lab_title, $proposal_data->department, $category_data, l('Edit Category', 'lab_migration/manage_proposal/category/edit/' . $proposal_data->id));
  }

  $proposal_header = array('Date of Submission', 'Name', 'Title of the Lab', 'Department', 'Category', 'Action');
  $output = theme_table($proposal_header, $proposal_rows);
  return $output;
}

/******************************************************************************/
/************************** PROPOSAL APPROVAL FORM ****************************/
/******************************************************************************/

function proposal_approval_form($form_state)
{
  global $user;

  /* get current proposal */
  $proposal_id = (int)arg(3);
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  if ($proposal_q)
  {
    if ($proposal_data = db_fetch_object($proposal_q))
    {
      /* everything ok */
    } else {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('lab_migration/manage_proposal');
      return;
    }
  } else {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('lab_migration/manage_proposal');
    return;
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

  /* get experiment details */
  $experiment_list = '<ul>';
  $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d ORDER BY id ASC", $proposal_id);
  while ($experiment_data = db_fetch_object($experiment_q))
  {
    $experiment_list .= '<li>' . $experiment_data->title . '</li>';
  }
  $experiment_list .= '</ul>';

  $form['experiment'] = array(
      '#type' => 'item',
      '#value' => $experiment_list,
      '#title' => t('Experiments'),
  );

  if ($proposal_data->solution_provider_uid == 0) {
  	$solution_provider = "User will not provide solution, we will have to provide solution";
  } else if ($proposal_data->solution_provider_uid == $proposal_data->uid) {
  	$solution_provider = "Proposer will provide the solution of the lab";
  } else {
  	$solution_provider_user_data = user_load($proposal_data->solution_provider_uid);
  	if ($solution_provider_user_data)
  		$solution_provider = "Solution will be provided by user " . l($solution_provider_user_data->name, 'user/' . $proposal_data->solution_provider_uid);
  	else
  		$solution_provider = "User does not exists";
  }
  $form['solution_provider_uid'] = array(
	  '#type' => 'item',
		'#title' => t('Do you want to provide the solution'),
		'#value' => $solution_provider,
  );

  $form['solution_display'] = array(
	  '#type' => 'item',
		'#title' => t('Do you want to display the solution on the www.scilab.in website'),
		'#value' => ($proposal_data->solution_display == 1) ? "Yes" : "No",
  );

  $form['approval'] = array(
	  '#type' => 'radios',
		'#title' => t('Lab migration proposal'),
		'#options' => array('1' => 'Approve', '2' => 'Disapprove'),
		'#required' => TRUE,
	);

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
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  if ($proposal_q)
  {
    if ($proposal_data = db_fetch_object($proposal_q))
    {
      /* everything ok */
    } else {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('lab_migration/manage_proposal');
      return;
    }
  } else {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('lab_migration/manage_proposal');
    return;
  }

  if ($form_state['values']['approval'] == 1) {
  	db_query("UPDATE {lab_migration_proposal} SET approver_uid = %d, approval_date = %d, approval_status = 1 WHERE id = %d", $user->uid, time(), $proposal_id);

    /* sending email */
    $user_data = user_load($proposal_data->uid);
    $email_to = $user_data->mail;
    $param['proposal_approved']['proposal_id'] = $proposal_id;
    $param['proposal_approved']['user_id'] = $proposal_data->uid;
    if (!drupal_mail('lab_migration', 'proposal_approved', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
      drupal_set_message('Error sending email message.', 'error');

    $email_to = $user->mail . ', ' . variable_get('lab_migration_emails', '');
    if (!drupal_mail('lab_migration', 'proposal_approved', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
      drupal_set_message('Error sending email message.', 'error');

    drupal_set_message('Lab migration proposal No. ' . $proposal_id  . ' approved. User has been notified of the approval.', 'status');
    drupal_goto('lab_migration/manage_proposal');
    return;
  } else if ($form_state['values']['approval'] == 2) {
    db_query("UPDATE {lab_migration_proposal} SET approver_uid = %d, approval_date = %d, approval_status = 2, message = '%s' WHERE id = %d", $user->uid, time(), $form_state['values']['message'], $proposal_id);

    /* sending email */
    $user_data = user_load($proposal_data->uid);
    $email_to = $user_data->mail;
    $param['proposal_disapproved']['proposal_id'] = $proposal_id;
    $param['proposal_disapproved']['user_id'] = $proposal_data->uid;
    if (!drupal_mail('lab_migration', 'proposal_disapproved', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
      drupal_set_message('Error sending email message.', 'error');

    $email_to = $user->mail . ', ' . variable_get('lab_migration_emails', '');;
    if (!drupal_mail('lab_migration', 'proposal_disapproved', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
      drupal_set_message('Error sending email message.', 'error');

    drupal_set_message('Lab migration proposal No. ' . $proposal_id  . ' dis-approved. User has been notified of the dis-approval.', 'error');
    drupal_goto('lab_migration/manage_proposal');
    return;
  }
}


/******************************************************************************/
/*************************** PROPOSAL STATUS FORM *****************************/
/******************************************************************************/

function proposal_status_form($form_state)
{
  global $user;

  /* get current proposal */
  $proposal_id = (int)arg(3);
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  if ($proposal_q)
  {
    if ($proposal_data = db_fetch_object($proposal_q))
    {
      /* everything ok */
    } else {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('lab_migration/manage_proposal');
      return;
    }
  } else {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('lab_migration/manage_proposal');
    return;
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

  /* get experiment details */
  $experiment_list = '<ul>';
  $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d ORDER BY id ASC", $proposal_id);
  while ($experiment_data = db_fetch_object($experiment_q))
  {
    $experiment_list .= '<li>' . $experiment_data->title . '</li>';
  }
  $experiment_list .= '</ul>';

  $form['experiment'] = array(
      '#type' => 'item',
      '#value' => $experiment_list,
      '#title' => t('Experiments'),
  );

  if ($proposal_data->solution_provider_uid == 0) {
  	$solution_provider = "User will not provide solution, we will have to provide solution";
  } else if ($proposal_data->solution_provider_uid == $proposal_data->uid) {
  	$solution_provider = "Proposer will provide the solution of the lab";
  } else {
  	$solution_provider_user_data = user_load($proposal_data->solution_provider_uid);
  	if ($solution_provider_user_data)
  		$solution_provider = "Solution will be provided by user " . l($solution_provider_user_data->name, 'user/' . $proposal_data->solution_provider_uid);
  	else
  		$solution_provider = "User does not exists";
  }
  $form['solution_provider_uid'] = array(
	  '#type' => 'item',
		'#title' => t('Who will provide the solution'),
		'#value' => $solution_provider,
  );

  $form['solution_display'] = array(
	  '#type' => 'item',
		'#title' => t('Display the solution on the www.scilab.in website'),
		'#value' => ($proposal_data->solution_display == 1) ? "Yes" : "No",
  );

  $proposal_status = '';
  switch ($proposal_data->approval_status)
  {
  case 0: $proposal_status = t('Pending'); break;
  case 1: $proposal_status = t('Approved'); break;
  case 2: $proposal_status = t('Dis-approved'); break;
  case 3: $proposal_status = t('Completed'); break;
  default: $proposal_status = t('Unkown'); break;
  }
  $form['proposal_status'] = array(
      '#type' => 'item',
      '#value' => $proposal_status,
      '#title' => t('Proposal Status'),
  );

  if ($proposal_data->approval_status == 0)
  {
    $form['approve'] = array(
      '#type' => 'item',
      '#value' => l('Click here', 'lab_migration/manage_proposal/approve/' . $proposal_id),
      '#title' => t('Approve'),
    );
  }

  if ($proposal_data->approval_status == 1)
  {
    $form['completed'] = array(
      '#type' => 'checkbox',
      '#title' => t('Completed'),
      '#description' => t('Check if user has provided all experiment solutions.'),
    );
  }

  if ($proposal_data->approval_status == 2) {
    $form['message'] = array(
      '#type' => 'item',
      '#value' => $proposal_data->message,
      '#title' => t('Reason for disapproval'),
    );
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );

  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'lab_migration/manage_proposal/all'),
  );

  return $form;
}

function proposal_status_form_submit($form, &$form_state)
{
  global $user;

  /* get current proposal */
  $proposal_id = (int)arg(3);
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  if ($proposal_q)
  {
    if ($proposal_data = db_fetch_object($proposal_q))
    {
      /* everything ok */
    } else {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('lab_migration/manage_proposal');
      return;
    }
  } else {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('lab_migration/manage_proposal');
    return;
  }

  /* set the book status to completed */
  if ($form_state['values']['completed'] == 1)
  {
    db_query("UPDATE {lab_migration_proposal} SET approval_status = 3 WHERE id = %d", $proposal_id);

    /* sending email */
    $user_data = user_load($proposal_data->uid);
    $email_to = $user_data->mail;
    $param['proposal_completed']['proposal_id'] = $proposal_id;
    $param['proposal_completed']['user_id'] = $proposal_data->uid;
    if (!drupal_mail('lab_migration', 'proposal_completed', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
      drupal_set_message('Error sending email message.', 'error');

    $email_to = $user->mail . ', ' . variable_get('lab_migration_emails', '');;
    if (!drupal_mail('lab_migration', 'proposal_completed', $email_to , language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
      drupal_set_message('Error sending email message.', 'error');
 
    drupal_set_message('Congratulations! Lab Migration proposal has been marked as completed. User has been notified of the completion.', 'status');
  }
  drupal_goto('lab_migration/manage_proposal');
  return;
}


/******************************************************************************/
/**************************** PROPOSAL EDIT FORM ******************************/
/******************************************************************************/

function proposal_edit_form($form_state)
{
  global $user;

  /* get current proposal */
  $proposal_id = (int)arg(3);
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  if ($proposal_q)
  {
    if ($proposal_data = db_fetch_object($proposal_q))
    {
      /* everything ok */
    } else {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('lab_migration/manage_proposal');
      return;
    }
  } else {
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
    '#default_value' => $proposal_data->name_title,
  );
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Name of the Proposer'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $proposal_data->name,
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
    '#default_value' => $proposal_data->contact_ph,
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
    '#default_value' => $proposal_data->department,
  );
  $form['university'] = array(
    '#type' => 'textfield',
    '#title' => t('University/Institute'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $proposal_data->university,
  );
  $form['lab_title'] = array(
    '#type' => 'textfield',
    '#title' => t('Title of the Lab'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
    '#default_value' => $proposal_data->lab_title,
  );

  /* get experiment details */
  $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d ORDER BY id ASC", $proposal_id);
			$form['lab_experiment'] = array(
				'#type' => 'fieldset',
				'#collapsible' => FALSE,
				'#tree' => TRUE,
			);
  for ($counter = 1; $counter <= 15; $counter++) {
  	$experiment_title = '';
  	$experiment_data = db_fetch_object($experiment_q);
  	if ($experiment_data) {
  		$experiment_title = $experiment_data->title;
			$form['lab_experiment']['update'][$experiment_data->id] = array(
				'#type' => 'textfield',
				'#title' => t('Title of the Experiment ') . $counter,
				'#size' => 50,
				'#required' => FALSE,
				'#tree' => TRUE,
				'#default_value' => $experiment_title,
			);
  	} else {
			$form['lab_experiment']['insert'][$counter] = array(
				'#type' => 'textfield',
				'#title' => t('Title of the Experiment ') . $counter,
				'#size' => 50,
				'#required' => FALSE,
				'#default_value' => $experiment_title,
			);
		}
	}

	if ($proposal_data->solution_provider_uid == 0)
		$solution_provider_user = 1;
	else if ($proposal_data->solution_provider_uid == $proposal_data->uid)
		$solution_provider_user = 2;
	else
		$solution_provider_user = 3;
  $form['solution_provider_uid'] = array(
	  '#type' => 'radios',
		'#title' => t('Who will provide the solution'),
		'#options' => array('1' => 'Open', '2' => 'Proposer', '3' => 'User id mentioned below'),
		'#required' => TRUE,
		'#default_value' => $solution_provider_user,
  );
  $form['solution_provider_user_id'] = array(
	  '#type' => 'textfield',
		'#default_value' => ($solution_provider_user == 3) ? $proposal_data->solution_provider_uid : "",
  );

  $form['solution_display'] = array(
	  '#type' => 'radios',
		'#title' => t('Do you want to display the solution on the www.scilab.in website'),
		'#options' => array('1' => 'Yes', '2' => 'No'),
		'#required' => TRUE,
		'#default_value' => ($proposal_data->solution_display == 1) ? "1" : "2",
  );

  $form['delete_proposal'] = array(
    '#type' => 'checkbox',
    '#title' => t('Delete Proposal')
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

function proposal_edit_form_validate($form, &$form_state)
{
  if ($form_state['values']['solution_provider_uid'] == 3) {
  	if (!(user_load((int)$form_state['values']['solution_provider_user_id']))) {
  		form_set_error('solution_provider_user_id', t('Solution provider user id does not exists'));
  	}
  }
}

function proposal_edit_form_submit($form, &$form_state)
{
  global $user;

  /* get current proposal */
  $proposal_id = (int)arg(3);
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  if ($proposal_q)
  {
    if ($proposal_data = db_fetch_object($proposal_q))
    {
      /* everything ok */
    } else {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('lab_migration/manage_proposal');
      return;
    }
  } else {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('lab_migration/manage_proposal');
    return;
  }

  /* delete proposal */
  if ($form_state['values']['delete_proposal'] == 1) {
		db_query("DELETE FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
		db_query("DELETE FROM {lab_migration_experiment} WHERE proposal_id = %d", $proposal_id);
		drupal_set_message(t('Proposal Delete'), 'status');
		drupal_goto('lab_migration/manage_proposal');
		return;
  }

  $solution_provider_uid = 0;
  if ($form_state['values']['solution_provider_uid'] == 1) {
  	$solution_provider_uid = 0;
  } else if ($form_state['values']['solution_provider_uid'] == 2) {
  	$solution_provider_uid = $proposal_data->uid;
  } else if ($form_state['values']['solution_provider_uid'] == 3) {
  	$solution_provider_uid = (int)$form_state['values']['solution_provider_user_id'];
  }

  $solution_display = 0;
  if ($form_state['values']['solution_display'] == 1) {
  	$solution_display = 1;
  } else {
  	$solution_display = 0;
  }

  /* update proposal */
  $result = db_query("UPDATE {lab_migration_proposal} SET name_title = '%s', name = '%s', contact_ph = '%s', department = '%s', university = '%s', lab_title = '%s', solution_provider_uid = %d, solution_display = %d WHERE id = %d",
    $form_state['values']['name_title'],
    $form_state['values']['name'],
    $form_state['values']['contact_ph'],
    $form_state['values']['department'],
    $form_state['values']['university'],
    $form_state['values']['lab_title'],
    $solution_provider_uid,
    $solution_display,
    $proposal_id);
  if (!$result)
  {
    drupal_set_message(t('Error receiving your proposal. Please try again.'), 'error');
    return;
  }

  /* updating existing experiments */
  foreach ($form_state['values']['lab_experiment']['update'] as $update_id => $update_value) {
  	if (strlen(trim($update_value)) >= 1) {
  		$result = db_query("UPDATE {lab_migration_experiment} SET title = '%s' WHERE id = %d", trim($update_value), $update_id);
			if (!$result)
			{
				drupal_set_message(t('Could not update Title of the Experiment : ') . trim($update_value), 'error');
			}
		} else {
			db_query("DELETE FROM {lab_migration_experiment} WHERE id = %d LIMIT 1", $update_id);
		}
	}

  /* inserting new experiments */
  foreach ($form_state['values']['lab_experiment']['insert'] as $insert_id => $insert_value) {
  	if (strlen(trim($insert_value)) >= 1) {
  		$result = db_query("INSERT INTO {lab_migration_experiment} (proposal_id, title) VALUES (%d, '%s')", $proposal_id, trim($insert_value));
			if (!$result)
			{
				drupal_set_message(t('Could not insert Title of the Experiment : ') . trim($insert_value), 'error');
			}
		}
	}

  drupal_set_message(t('Proposal Updated'), 'status');
}



/******************************************************************************/
/**************************** CATEGORY EDIT FORM ******************************/
/******************************************************************************/

function category_edit_form($form_state)
{
  /* get current proposal */
  $proposal_id = (int)arg(4);
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  if ($proposal_q)
  {
    if ($proposal_data = db_fetch_object($proposal_q))
    {
      /* everything ok */
    } else {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('lab_migration/manage_proposal');
      return;
    }
  } else {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('lab_migration/manage_proposal');
    return;
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

  $form['category'] = array(
    '#type' => 'select',
    '#title' => t('Category'),
    '#options' => array(0 => 'Please select',
                        1 => 'Fluid Mechanics',
                        2 => 'Control Theory & Control Systems',
                        3 => 'Chemical Engineering',
                        4 => 'Thermodynamics',
                        5 => 'Mechanical Engineering',
                        6 => 'Signal Processing',
                        7 => 'Digital Communications',
                        8 => 'Electrical Technology',
                        9 => 'Mathematics & Pure Science',
                        10 => 'Analog Electronics',
                        11 => 'Digital Electronics',
                        12 => 'Computer Programming',
                        13 => 'Others'),
    '#required' => TRUE,
    '#default_value' => $proposal_data->category,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );
  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'lab_migration/manage_proposal/category'),
  );
  return $form;
}

function category_edit_form_submit($form, &$form_state)
{
  /* get current proposal */
  $proposal_id = (int)arg(4);
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  if ($proposal_q)
  {
    if ($proposal_data = db_fetch_object($proposal_q))
    {
      /* everything ok */
    } else {
      drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
      drupal_goto('lab_migration/manage_proposal');
      return;
    }
  } else {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('lab_migration/manage_proposal');
    return;
  }

  db_query("UPDATE {lab_migration_proposal} SET category = %d WHERE id = %d", $form_state['values']['category'], $proposal_data->id);

  drupal_set_message(t('Proposal Category Updated'), 'status');
  drupal_goto('lab_migration/manage_proposal/category');
}

