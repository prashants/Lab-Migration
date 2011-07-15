<?php
// $Id$

function lab_migration_proposal_form($form_state)
{
  global $user;

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
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
  );
  $form['problem_topic'] = array(
    '#type' => 'textfield',
    '#title' => t('Topic of the Problem'),
    '#size' => 30,
    '#maxlength' => 50,
    '#required' => TRUE,
  );
  $form['problem_file'] = array(
		'#type' => 'file',
		'#title' => t('Upload problem statement'),
		'#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' .
		t('Allowed file extensions : ') . variable_get('lab_migration_uploads_extensions', ''),
  );
  $form['supplementary_file'] = array(
		'#type' => 'file',
		'#title' => t('Upload supplementary files (eg. existing soultion of problem in any other software)'),
		'#description' => t('If more than one file, then compress and upload as .zip or .tar file') . '<br />' .
			t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' .
		t('Allowed file extensions : ') . variable_get('lab_migration_uploads_extensions', ''),
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

  if ( ! ($_FILES['files']['name']['problem_file'])) {
    form_set_error('problem_file', t('Problem statement file is required.'));
  } else {
  	$allowed_extensions_str = variable_get('lab_migration_uploads_extensions', '');
		$allowed_extensions = explode(',' , $allowed_extensions_str);
		$temp_extension = end(explode('.', strtolower($_FILES['files']['name']['problem_file'])));
		if (!in_array($temp_extension, $allowed_extensions))
			form_set_error('problem_file', t('Only file with ' . $allowed_extensions_str . ' extensions can be uploaded.'));
		if ($_FILES['files']['size']['problem_file'] <= 0)
			form_set_error('problem_file', t('File size cannot be zero.'));

		/* check if valid file name */
		if (!lab_migration_check_valid_filename($_FILES['files']['name']['problem_file']))
		 form_set_error('problem_file', t('Invalid file name specified. Only alphabets, numbers and underscore is allowed as a valid filename.'));
  }

  if ($_FILES['files']['name']['supplementary_file']) {
		$allowed_extensions_str = variable_get('lab_migration_uploads_extensions', '');
		$allowed_extensions = explode(',' , $allowed_extensions_str);
		$temp_extension = end(explode('.', strtolower($_FILES['files']['name']['supplementary_file'])));
		if (!in_array($temp_extension, $allowed_extensions))
			form_set_error('supplementary_file', t('Only file with ' . $allowed_extensions_str . ' extensions can be uploaded.'));
		if ($_FILES['files']['size']['supplementary_file'] <= 0)
			form_set_error('supplementary_file', t('File size cannot be zero.'));

		/* check if valid file name */
		if (!lab_migration_check_valid_filename($_FILES['files']['name']['supplementary_file']))
		 form_set_error('supplementary_file', t('Invalid file name specified. Only alphabets, numbers and underscore is allowed as a valid filename.'));

	 	/* if file already exists */
	 	if ($_FILES['files']['name']['problem_file'] == $_FILES['files']['name']['supplementary_file']) {
	 		form_set_error('supplementary_file', t('Supplementary file cannot be same as the problem statement file.'));
	 	}
	}
  return;
}

function lab_migration_proposal_form_submit($form, &$form_state)
{
  global $user;
  $root_path = lab_migration_path();

  /* inserting the user proposal */
  $result = db_query("INSERT INTO {lab_migration_proposal}
    (uid, approver_uid, solver_uid, name_title, name, contact_ph, department, university, lab_title, problem_topic, approval_status, solution_status, creation_date, approval_date, solution_date) VALUES
    (%d, %d, 0, '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, 0, %d, %d, 0)",
    $user->uid,
    0,
    $form_state['values']['name_title'],
    $form_state['values']['name'],
    $form_state['values']['contact_ph'],
    $form_state['values']['department'],
    $form_state['values']['university'],
    $form_state['values']['lab_title'],
    $form_state['values']['problem_topic'],
    0,
    time(),
    0
  );
  if (!$result)
  {
    drupal_set_message(t('Error receiving your proposal. Please try again.'), 'error');
    return;
  }
  /* proposal id */
  $proposal_id = db_last_insert_id('lab_migration_proposal', 'id');

  /************** uploading file *******************/
  /* creating directories */
  $dest_path =  $proposal_id . '/';
  if (!is_dir($root_path . $dest_path))
    mkdir($root_path . $dest_path);

	if (file_exists($root_path . $dest_path . $_FILES['files']['name']['problem_file']))
	{
		drupal_set_message(t("Error uploading file. File !filename already exists.", array('!filename' => $_FILES['files']['name']['problem_file'])), 'error');
		return;
	}

	/* uploading file */
	$filename = $_FILES['files']['name']['problem_file'];
	if (move_uploaded_file($_FILES['files']['tmp_name']['problem_file'], $root_path . $dest_path . $filename))
	{
		/* for uploaded files making an entry in the database */
		db_query("INSERT INTO {lab_migration_files} (link_id, filename, filepath, filemime, filesize, filetype, timestamp)
			VALUES (%d, '%s', '%s', '%s', %d, '%s', %d)",
			$proposal_id,
			$filename,
			$dest_path . $filename,
			$_FILES['files']['type']['problem_file'],
			$_FILES['files']['size']['problem_file'],
			'P',
			time()
			);
	} else {
		drupal_set_message('Error uploading file : ' . $dest_path . '/' . $filename, 'error');
	}

	/* uploading file */
	if ($_FILES['files']['name']['supplementary_file']) {
		$filename = $_FILES['files']['name']['supplementary_file'];
		if (move_uploaded_file($_FILES['files']['tmp_name']['supplementary_file'], $root_path . $dest_path . $filename))
		{
			/* for uploaded files making an entry in the database */
			db_query("INSERT INTO {lab_migration_files} (link_id, filename, filepath, filemime, filesize, filetype, timestamp)
				VALUES (%d, '%s', '%s', '%s', %d, '%s', %d)",
				$proposal_id,
				$filename,
				$dest_path . $filename,
				$_FILES['files']['type']['supplementary_file'],
				$_FILES['files']['size']['supplementary_file'],
				'S',
				time()
				);
		} else {
			drupal_set_message('Error uploading file : ' . $dest_path . '/' . $filename, 'error');
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

  drupal_set_message(t('We have received you lab migration proposal. We will get back to you soon.'), 'status');
  drupal_goto('');
}

