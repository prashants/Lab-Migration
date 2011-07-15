<?php
// $Id$

function _proposal_pending_solution()
{
  /* get pending proposals to be approved */
  $pending_rows = array();
  $pending_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE approval_status = 1 AND solution_status = 0 ORDER BY id DESC");
  while ($pending_data = db_fetch_object($pending_q)) {
    $pending_rows[$pending_data->id] = array(date('d-m-Y', $pending_data->creation_date), l($pending_data->name_title . ' ' . $pending_data->name, 'user/' . $pending_data->uid), $pending_data->department, $pending_data->university, $pending_data->lab_title, l('Upload Solution', '/lab_migration/manage_proposal/solution/' . $pending_data->id));
  }

  /* check if there are any pending proposals */
  if (!$pending_rows) {
    drupal_set_message(t('There are no pending proposals to be solved.'), 'status');
    return '';
  }

  $pending_header = array('Date of Submission', 'Name of the Proposer', 'Department/Branch', 'University/Institute', 'Title of the Lab', 'Action');
  $output = theme_table($pending_header, $pending_rows);
  return $output; 
}

/******************************************************************************/
/************************** PROPOSAL SOLUTION FORM ****************************/
/******************************************************************************/

function proposal_solution_form($form_state)
{
  global $user;
  $dl_root_path = 'sites/default/files/lab_migration/';

  /* get current proposal */
  $proposal_id = arg(3);
  $result = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  if (!($row = db_fetch_object($result))) {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('lab_migration/manage_proposal/pending_proposal');
    return;
  }
  if ($row->approval_status != 1) {
    drupal_set_message(t('Selected Proposal has not been approved yet.'), 'error');
    drupal_goto('lab_migration/manage_proposal/pending_proposal');
    return;
  }
	if ($row->solution_status != 0) {
		drupal_set_message(t('Selected Proposal has already been solved. You can reupload a new solution.'), 'status');
	}

  $form['#attributes'] = array('enctype' => "multipart/form-data");

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

	$approver_user_data = user_load($row->approver_uid );
	$form['approval_details'] = array(
		'#type' => 'item',
		'#value' => 'By : ' . l($approver_user_data->name, 'user/' . $row->approver_uid) . '<br />'
								. 'On : ' . date('d-m-Y', $row->approval_date),
	);

  $sol_file_q = db_query("SELECT * FROM {lab_migration_files} WHERE link_id = %d AND filetype = 'A'", $row->id);
  $sol_file_data = db_fetch_object($sol_file_q);
  if ($sol_file_data) {
		$form['sol_file'] = array(
			'#type' => 'item',
			'#value' => l($sol_file_data->filename, $dl_root_path . $sol_file_data->filepath),
			'#title' => t('Solution file'),
		);
	}

  $form['solution_file'] = array(
		'#type' => 'file',
		'#title' => t('Upload solution statement'),
		'#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' .
		t('Allowed file extensions : ') . variable_get('lab_migration_uploads_extensions', ''),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );

  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'lab_migration/manage_proposal/pending_solution'),
  );

  return $form;
}

function proposal_solution_form_validate($form, &$form_state)
{
	$proposal_id = arg(3);

  if ( ! ($_FILES['files']['name']['solution_file'])) {
    form_set_error('problem_file', t('Solution statement file is required.'));
  } else {
  	$allowed_extensions_str = variable_get('lab_migration_uploads_extensions', '');
		$allowed_extensions = explode(',' , $allowed_extensions_str);
		$temp_extension = end(explode('.', strtolower($_FILES['files']['name']['solution_file'])));
		if (!in_array($temp_extension, $allowed_extensions))
			form_set_error('solution_file', t('Only file with ' . $allowed_extensions_str . ' extensions can be uploaded.'));
		if ($_FILES['files']['size']['solution_file'] <= 0)
			form_set_error('solution_file', t('File size cannot be zero.'));

		/* check if valid file name */
		if (!lab_migration_check_valid_filename($_FILES['files']['name']['solution_file']))
		 form_set_error('solution_file', t('Invalid file name specified. Only alphabets, numbers and underscore is allowed as a valid filename.'));

	 	/* if file already exists */
	 	$file_exists_q = db_query("SELECT * FROM {lab_migration_files} WHERE link_id = %d AND filename = '%s'", $proposal_id, $_FILES['files']['name']['solution_file']);
	 	if (db_fetch_object($file_exists_q))
			form_set_error('solution_file', t('File with the same name already exists. Please rename the solution file and try again.'));
	}
  return;
}

function proposal_solution_form_submit($form, &$form_state)
{
  global $user;
  $root_path = lab_migration_path();

  /* get current proposal */
  $proposal_id = (int)arg(3);
  $result = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  if (!($row = db_fetch_object($result))) {
    drupal_set_message(t('Invalid proposal selected. Please try again.'), 'error');
    drupal_goto('lab_migration/manage_proposal');
    return;
  }
  if ($row->approval_status != 1) {
    drupal_set_message(t('Selected Proposal has not been approved yet.'), 'error');
    drupal_goto('lab_migration/manage_proposal/pending_solution');
    return;
  }

	if ($row->solution_status == 1) {
  	$files_q = db_query("SELECT * FROM {lab_migration_files} WHERE link_id = %d AND filetype = 'A'", $proposal_id);
  	while ($file_data = db_fetch_object($files_q)) {
			if (!file_exists($root_path . $file_data->filepath)) {
				drupal_set_message(t('Error deleting !file. File does not exists.', array('!file' => $file_data->filepath)), 'error');
				return;
			}
			/* removing file */
			if (!unlink($root_path . $file_data->filepath)) {
				drupal_set_message(t('Error deleting !file', array('!file' => $file_data->filepath)), 'error');
				return;
			} else {
				/* deleting example files database entries */
				db_query("DELETE FROM {lab_migration_files} WHERE id = %d", $file_data->id);
			}
		}
	}

  /************** uploading file *******************/
  /* creating directories */
  $dest_path =  $proposal_id . '/';
  if (!is_dir($root_path . $dest_path))
    mkdir($root_path . $dest_path);

	if (file_exists($root_path . $dest_path . $_FILES['files']['name']['solution_file']))
	{
		drupal_set_message(t("Error uploading file. File !filename already exists.", array('!filename' => $_FILES['files']['name']['solution_file'])), 'error');
		return;
	}

	/* uploading file */
	$filename = $_FILES['files']['name']['solution_file'];
	if (move_uploaded_file($_FILES['files']['tmp_name']['solution_file'], $root_path . $dest_path . $filename))
	{
		/* for uploaded files making an entry in the database */
		db_query("INSERT INTO {lab_migration_files} (link_id, filename, filepath, filemime, filesize, filetype, timestamp)
			VALUES (%d, '%s', '%s', '%s', %d, '%s', %d)",
			$proposal_id,
			$filename,
			$dest_path . $filename,
			$_FILES['files']['type']['solution_file'],
			$_FILES['files']['size']['solution_file'],
			'A',
			time()
			);

		db_query("UPDATE {lab_migration_proposal} SET solution_status = 1, solver_uid = %d, solution_date = %d WHERE id = %d", $user->uid, time(), $proposal_id);
	} else {
		drupal_set_message('Error uploading file : ' . $dest_path . '/' . $filename, 'error');
	}

  drupal_goto('lab_migration/manage_proposal/pending_solution');
}

