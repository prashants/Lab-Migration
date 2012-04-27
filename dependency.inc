<?php
// $Id$

function upload_dependency_form($form_state)
{
  global $user;

  $proposal_data = get_proposal();
  if (!$proposal_data) {
      drupal_goto('');
      return;
  }

  $form['#attributes'] = array('enctype' => "multipart/form-data");

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

  $form['existing_depfile'] = array(
    '#type' => 'item',
    '#value' => _list_existing_dependency($proposal_data->id),
    '#title' => t('List of existing dependency files for this book'),
  );

  $form['depfile'] = array(
    '#type' => 'fieldset',
    '#title' => t('Upload Dependency Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['depfile']['depfile1'] = array(
      '#type' => 'file',
      '#title' => t('Upload dependency file'),
      '#description' => t("Allowed file extensions : ") . variable_get('lab_migration_dependency_extensions', ''),
  );
  $form['depfile']['depfile1_caption'] = array(
      '#type' => 'textfield',
      '#title' => t('Caption for dependency file'),
      '#size' => 15,
      '#maxlength' => 100,
      '#required' => TRUE,
  );
  $form['depfile']['depfile1_description'] = array(
      '#type' => 'textarea',
      '#title' => t('Brief Description of the dependency file'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );

  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Back'), 'lab_migration/code'),
  );
  return $form;
}

function upload_dependency_form_validate($form, &$form_state)
{
  global $user;

  /* get approved proposal details */
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE uid = %d ORDER BY id DESC LIMIT 1", $user->uid);
  $proposal_data = db_fetch_object($proposal_q);
  if (!$proposal_data)
  {
  	form_set_error('', t('Invalid'));
  }

  if (isset($_FILES['files']))
  {
    /* check for valid filename extensions */  
    $allowed_extensions = explode(',' , variable_get('lab_migration_dependency_extensions', ''));
    foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
    {
      if ($file_name)
      {
         $temp_extension = end(explode('.', strtolower($_FILES['files']['name'][$file_form_name])));
         if (!in_array($temp_extension, $allowed_extensions))
           form_set_error($file_form_name, t('Only ' . variable_get('lab_migration_dependency_extensions', '') . ' extensions can be uploaded.'));
         if ($_FILES['files']['size'][$file_form_name] <= 0)
           form_set_error($file_form_name, t('File size cannot be zero.'));

         /* check if file already exists */
         $dep_exists_data = db_fetch_object(db_query("SELECT * FROM {lab_migration_dependency_files} WHERE filename = '%s'", $_FILES['files']['name'][$file_form_name]));
         if ($dep_exists_data)
           form_set_error($file_form_name, t('Dependency file with the same name has already been uploaded in this or some other lab solution. Please rename the file and try again.'));

         /* check if valid file name */
         if (!lab_migration_check_valid_filename($_FILES['files']['name'][$file_form_name]))
           form_set_error($file_form_name, t('Invalid file name specified. Only alphabets, numbers and underscore is allowed as a valid filename.'));
      }
    }
  }
}

function upload_dependency_form_submit($form, &$form_state) {
  global $user;

  $root_path = lab_migration_path();

  $proposal_data = get_proposal();
  if (!$proposal_data) {
      drupal_goto('');
      return;
  }

  $dest_path .= 'DEPENDENCIES' . '/';
  if (!is_dir($root_path . $dest_path))
    mkdir($root_path . $dest_path);

  /* uploading dependencies */
  $file_upload_counter = 0;
  $dependency_ids = array();
  $dependency_names = array();

  foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
  {
    if ($file_name)
    {
      /* uploading file */
      if (move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
      {
        /* for uploaded files making an entry in the database */
        db_query("INSERT INTO {lab_migration_dependency_files} (proposal_id, filename, filepath, filemime, filesize, caption, description, timestamp)
          VALUES (%d, '%s', '%s', '%s', %d, '%s', '%s', %d)",
          $proposal_data->id,
          $_FILES['files']['name'][$file_form_name],
          $dest_path . $_FILES['files']['name'][$file_form_name],
          $_FILES['files']['type'][$file_form_name],
          $_FILES['files']['size'][$file_form_name],
          check_plain($form_state['values'][$file_form_name . '_caption']),
          check_plain($form_state['values'][$file_form_name . '_description']),
          time()
          );
        drupal_set_message($file_name . ' uploaded successfully.', 'status');
        $dependency_ids[] = db_last_insert_id('lab_migration_dependency_files', 'id');
        $dependency_names[] =  $_FILES['files']['name'][$file_form_name];
        $file_upload_counter++;
      } else {
        drupal_set_message('Error uploading dependency : ' . $dest_path . '/' . $_FILES['files']['name'][$file_form_name], 'error');
      }
    }
  }

  if ($file_upload_counter > 0)
  {
    drupal_set_message('Dependencies uploaded successfully.', 'status');
  
    /* sending email */
    $param['dependency_uploaded']['user_id'] = $user->uid;
    $param['dependency_uploaded']['dependency_names'] = $dependency_names;

    $email_to = $user->mail . ', ' . variable_get('lab_migration_emails', '');
    if (!drupal_mail('lab_migration', 'dependency_uploaded', $email_to, language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
      drupal_set_message('Error sending email message.', 'error');
  }

  drupal_goto('lab_migration/code/upload_dep');
}

function _list_existing_dependency($proposal_id)
{
  $return_html = '<ul>';
  $proposal_dependency_files_q = db_query("SELECT * FROM {lab_migration_dependency_files} WHERE proposal_id = %d ORDER BY filename ASC", $proposal_id);

  $counter = 0;
  while ($proposal_dependency_files_data = db_fetch_object($proposal_dependency_files_q))
  {
    $temp_caption = '';
    if ($proposal_dependency_files_data->caption)
      $temp_caption = ' (' . $proposal_dependency_files_data->caption . ')';
    $return_html .= '<li>' . l($proposal_dependency_files_data->filename . $temp_caption, 'download/dependency/' . $proposal_dependency_files_data->id) . '</li>';
    $counter++;
  }
  if ($counter == 0)
    $return_html .= '<li>(None)</li>';
  $return_html .= '</ul>';
  return $return_html;
}

