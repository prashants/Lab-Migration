<?php
// $Id$

function code_approval()
{
  /* get a list of unapproved solutions */
  $pending_solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE approval_status = 0");
  if (!$pending_solution_q)
  {
    drupal_set_message(t('There are no pending code approvals.'), 'status');
    return '';
  }
  $pending_solution_rows = array();
  while ($pending_solution_data = db_fetch_object($pending_solution_q))
  {
  	/* get experiment data */
    $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE id = %d", $pending_solution_data->experiment_id);
    $experiment_data = db_fetch_object($experiment_q);

    /* get proposal data */
    $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $experiment_data->proposal_id);
    $proposal_data = db_fetch_object($proposal_q);

    /* get solution provider details */
    $solution_provider_user_name = '';
    $user_data = user_load($proposal_data->solution_provider_uid);
    if ($user_data) {
    	$solution_provider_user_name = $user_data->name;
    } else {
    	$solution_provider_user_name = '';
    }

    /* setting table row information */
    $pending_solution_rows[] = array($proposal_data->lab_title, $experiment_data->title, $proposal_data->name, $solution_provider_user_name, l('Edit', 'lab_migration/code_approval/approve/' . $pending_solution_data->id));
  }

  /* check if there are any pending solutions */                                   
  if (!$pending_solution_rows)
  {
    drupal_set_message(t('There are no pending solutions'), 'status');
    return '';
  }

  $header = array('Title of the Lab', 'Experiment', 'Proposer', 'Solution Provider', 'Actions');
  $output = theme_table($header, $pending_solution_rows);
  return $output; 
}

function code_approval_form($form_state)
{
  $solution_id = (int)arg(3);

  /* get solution details */
  $solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE id = %d", $solution_id);
  $solution_data = db_fetch_object($solution_q);
  if (!$solution_data)
  {
    drupal_set_message(t('Invalid solution selected.'), 'status');
    drupal_goto('lab_migration/code_approval');
  }
  if ($solution_data->approval_status == 1) {
  	 drupal_set_message(t('This solution has already been approved. Are you sure you want to change the approval status?'), 'error');
  }
  if ($solution_data->approval_status == 2) {
  	 drupal_set_message(t('This solution has already been dis-approved. Are you sure you want to change the approval status?'), 'error');
  }

	/* get experiment data */
	$experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE id = %d", $solution_data->experiment_id);
	$experiment_data = db_fetch_object($experiment_q);

	/* get proposal data */
	$proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $experiment_data->proposal_id);
	$proposal_data = db_fetch_object($proposal_q);

	/* get solution provider details */
	$solution_provider_user_name = '';
	$user_data = user_load($proposal_data->solution_provider_uid);
	if ($user_data) {
		$solution_provider_user_name = $user_data->name;
	} else {
		$solution_provider_user_name = '';
	}

  $form['#tree'] = TRUE;

  $form['lab_title'] = array(
    '#type' => 'item',
    '#value' => $proposal_data->lab_title,
    '#title' => t('Title of the Lab'),
  );

  $form['name'] = array(
    '#type' => 'item',
    '#value' => $proposal_data->name,
    '#title' => t('Contributor Name'),
  );

  $form['experiment']['number'] = array(
    '#type' => 'item',
    '#value' => $experiment_data->number,
    '#title' => t('Experiment Number'),
  );

  $form['experiment']['title'] = array(
    '#type' => 'item',
    '#value' => $experiment_data->title,
    '#title' => t('Title of the Experiment'),
  );

  $form['back_to_list'] = array(
    '#type' => 'item',
    '#value' => l('Back to Code Approval List', 'lab_migration/code_approval'),
  );

  $form['code_number'] = array(
    '#type' => 'item',
    '#value' => $solution_data->code_number,
    '#title' => t('Code No'),
  );

  $form['code_caption'] = array(
    '#type' => 'item',
    '#value' => $solution_data->caption,
    '#title' => t('Caption'),
  );

	/* get solution files */
	$solution_files_html = '';
	$solution_files_q = db_query("SELECT * FROM {lab_migration_solution_files} WHERE solution_id = %d ORDER BY id ASC", $solution_id);
	if ($solution_files_q) {
		while ($solution_files_data = db_fetch_object($solution_files_q)) {
			$code_file_type = '';
			switch ($solution_files_data->filetype) {
			case 'S': $code_file_type = 'Source'; break;
			case 'R': $code_file_type = 'Result'; break;
			case 'X': $code_file_type = 'Xcox'; break;
			case 'U': $code_file_type = 'Unknown'; break;
			default: $code_file_type = 'Unknown'; break;
			}
			$solution_files_html .= l($solution_files_data->filename, '') . ' (' . $code_file_type . ')' . '<br/>';
		}
	}
	/* get dependencies files */
	$dependency_files_q = db_query("SELECT * FROM {lab_migration_dependency_files} WHERE proposal_id = %d ORDER BY id ASC", $proposal_data->id);
	if ($dependency_files_q) {
		while ($dependency_files_data = db_fetch_object($dependency_files_q)) {
			$solution_files_html .= l($dependency_files_data->filename, '') . ' (' . 'Dependency' . ')' . '<br/>';
		}
	}

  $form['solution_files'] = array(
    '#type' => 'item',
    '#value' => $solution_files_html,
    '#title' => t('Solution'),
  );

	$form['approved'] = array(
		'#type' => 'radios',
		'#options' => array('0' => 'Pending', '1' => 'Approved', '2' => 'Dis-approved (Solution will be deleted)'),
		'#title' => t('Approval'),
		'#default_value' => $solution_data->approval_status,
	);

	$form['message'] = array(
		'#type' => 'textarea',
		'#title' => t('Reason for dis-approval'),
	);

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );

  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'lab_migration/code_approval'),
  );

  return $form;
}

function code_approval_form_submit($form, &$form_state)
{
  global $user;

  $solution_id = (int)arg(3);

  /* get solution details */
  $solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE id = %d", $solution_id);
  $solution_data = db_fetch_object($solution_q);
  if (!$solution_data)
  {
    drupal_set_message(t('Invalid solution selected.'), 'status');
    drupal_goto('lab_migration/code_approval');
  }

	/* get experiment data */
	$experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE id = %d", $solution_data->experiment_id);
	$experiment_data = db_fetch_object($experiment_q);

	/* get proposal data */
	$proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $experiment_data->proposal_id);
	$proposal_data = db_fetch_object($proposal_q);

	$user_data = user_load($proposal_data->uid);
	$solution_prove_user_data = user_load($proposal_data->solution_provider_uid);

	// **** TODO **** : del_lab_pdf($proposal_data->id);

	if ($form_state['values']['approved'] == "0")
	{
		db_query("UPDATE {lab_migration_solution} SET approval_status = 0, approver_uid = %d, approval_date = %d WHERE id = %d", $user->uid, time(), $solution_id);

		/* sending email */
		$email_to = $user_data->mail . ', ' . variable_get('lab_migration_emails', '');
		$param['solution_pending']['solution_id'] = $solution_id;
		$param['solution_pending']['user_id'] = $user_data->uid;
		if (!drupal_mail('lab_migration', 'solution_pending', $email_to, language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
			drupal_set_message('Error sending email message.', 'error');
	} else if ($form_state['values']['approved'] == "1") {
		db_query("UPDATE {lab_migration_solution} SET approval_status = 1, approver_uid = %d, approval_date = %d WHERE id = %d", $user->uid, time(), $solution_id);

		/* sending email */
		$email_to = $user_data->mail . ', ' . variable_get('lab_migration_emails', '');
		$param['solution_approved']['solution_id'] = $solution_id;
		$param['solution_approved']['user_id'] = $user_data->uid;
		if (!drupal_mail('lab_migration', 'solution_pending', $email_to, language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
			drupal_set_message('Error sending email message.', 'error');
	} else if ($form_state['values']['approved'] == "2") {
		if (delete_solution($solution_id))
		{
			/* sending email */
			$email_to = $user_data->mail . ', ' . variable_get('lab_migration_emails', '');
			$param['solution_disapproved']['experiment_number'] = $experiment_data->number;
			$param['solution_disapproved']['experiment_title'] = $experiment_data->title;
			$param['solution_disapproved']['solution_code_number'] = $solution_data->code_number;
			$param['solution_disapproved']['solution_caption'] = $solution_data->caption;
			$param['solution_disapproved']['user_id'] = $user_data->uid;
			$param['solution_disapproved']['message'] = $form_state['values']['message'];
			if (!drupal_mail('lab_migration', 'solution_disapproved', $email_to, language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
				drupal_set_message('Error sending email message.', 'error');
		} else {
			drupal_set_message('Error disapproving and deleting solution. Please contact administrator.', 'error');
		}
	}

  drupal_set_message('Updated successfully.', 'status');
  drupal_goto('lab_migration/code_approval');
}

