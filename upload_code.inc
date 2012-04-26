<?php
// $Id$

function list_experiments()
{
  global $user;

  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE uid = %d ORDER BY id DESC LIMIT 1", $user->uid);
  $proposal_data = db_fetch_object($proposal_q);
  if (!$proposal_data)
  {
    drupal_set_message("Please submit a " . l('proposal', 'lab_migration/proposal') . ".", 'error');
    drupal_goto('');
  }
  if ($proposal_data->proposal_status  != 1)
  {
    switch ($proposal_data->approval_status )
    {
    case 0:
      drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
      drupal_goto('');
      return;
      break;
    case 1:
    	break;
    case 2:
      drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal ' . l('here', 'lab_migration/proposal') . '.'), 'error');
      drupal_goto('');
      return;
      break;
    case 3:
      drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'lab_migration/proposal') . '.'), 'status');
      drupal_goto('');
      return;
      break;
    default:
       drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
       drupal_goto('');
       return;
       break;
    }
  }

  $return_html = '<br />';
  $return_html .= '<strong>Title of the Experiment:</strong><br />' . $proposal_data->lab_title . '<br /><br />';
  $return_html .= '<strong>Proposer Name:</strong><br />' . $proposal_data->name_title . ' ' . $proposal_data->name . '<br /><br />';
  $return_html .= l('Upload Example Code', 'lab_migration/code/upload') . '<br />';

  /* get experiment list */
  $experiment_rows = array();
  $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d ORDER BY number ASC", $proposal_data->id);
  while ($experiment_data = db_fetch_object($experiment_q))
  {
    $experiment_rows[] = array($experiment_data->number, $experiment_data->title, '', '');
    /* get solution list */
    $solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE experiment_id = %d ORDER BY id ASC", $experiment_data->id);
    if ($solution_q) {
    	while ($solution_data = db_fetch_object($solution_q)) {
    		$solution_status = '';
    		switch ($solution_data->approval_status) {
    		case 0: $solution_status = "Pending"; break;
    		case 1: $solution_status = "Approved"; break;
    		default: $solution_status = "Unknown"; break;
    		}
    		$experiment_rows[] = array('', "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp" . $solution_data->code_number . "   " . $solution_data->caption, $solution_status, l('Delete', 'lab_migration/code/delete/' . $solution_data->id));
    		/* get solution files */
    		$solution_files_q = db_query("SELECT * FROM {lab_migration_solution_files} WHERE solution_id = %d ORDER BY id ASC", $solution_data->id);
    		if ($solution_files_q) {
    			while ($solution_files_data = db_fetch_object($solution_files_q)) {
    				$experiment_rows[] = array('', "&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp" . $solution_files_data->filename, '', '');
    			}
    		}
    	}
    }
  }

  $experiment_header = array('Exp. Number', 'Title of the Experiment', 'Status', 'Actions');
  $return_html .= theme_table($experiment_header, $experiment_rows);
  return $return_html;
}

function upload_code_form($form_state)
{
  global $user;

  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE uid = %d ORDER BY id DESC LIMIT 1", $user->uid);
  $proposal_data = db_fetch_object($proposal_q);
  if (!$proposal_data)
  {
    drupal_set_message("Please submit a " . l('proposal', 'lab_migration/proposal') . ".", 'error');
    drupal_goto('');
  }
  if ($proposal_data->proposal_status != 1)
  {
    switch ($proposal_data->approval_status )
    {
    case 0:
      drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
      drupal_goto('');
      return;
      break;
    case 1:
    	break;
    case 2:
      drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal ' . l('here', 'lab_migration/proposal') . '.'), 'error');
      drupal_goto('');
      return;
      break;
    case 3:
      drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'lab_migration/proposal') . '.'), 'status');
      drupal_goto('');
      return;
      break;
    default:
       drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
       drupal_goto('');
       return;
       break;
    }
  }

  /* add javascript for automatic book title, check if example uploaded, dependency selection effects */
  $dep_selection_js = " $(document).ready(function() {
    $('#edit-existing-depfile-dep-lab-title').change(function() {
      var dep_selected = '';
      /* showing and hiding relevant files */
      $('.form-checkboxes .option').hide();
      $('.form-checkboxes .option').each(function(index) {
        var activeClass = $('#edit-existing-depfile-dep-lab-title').val();
        if ($(this).children().hasClass(activeClass)) {
          $(this).show();
        }
        if ($(this).children().attr('checked') == true) {
          dep_selected += $(this).children().next().text() + '<br />';
        }
      });
      /* showing list of already existing dependencies */
      $('#existing_depfile_selected').html(dep_selected);
    });

    $('.form-checkboxes .option').change(function() {
      $('#edit-existing-depfile-dep-book-title').trigger('change');
    });
    $('#edit-existing-depfile-dep-book-title').trigger('change');
  });";
  drupal_add_js($dep_selection_js, 'inline', 'header');

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

  /* get experiment list */
  $experiment_rows = array();
  $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d ORDER BY id ASC", $proposal_data->id);
  while ($experiment_data = db_fetch_object($experiment_q))
  {
    $experiment_rows[$experiment_data->id] = $experiment_data->number . '. ' . $experiment_data->title;
  }
  $form['experiment'] = array(
    '#type' => 'select',
    '#title' => t('Title of the Experiment'),
    '#options' => $experiment_rows,
    '#multiple' => FALSE,
    '#size' => 1,
    '#required' => TRUE,
  );

  $form['code_number'] = array(
    '#type' => 'textfield',
    '#title' => t('Code No'),
    '#size' => 5,
    '#maxlength' => 10,
    '#description' => t(""),
    '#required' => TRUE,
  );
  $form['code_caption'] = array(
    '#type' => 'textfield',
    '#title' => t('Caption'),
    '#size' => 40,
    '#maxlength' => 255,
    '#description' => t(''),
    '#required' => TRUE,
  );
  $form['code_warning'] = array(
    '#type' => 'item',
    '#title' => t('You should upload all the files (main or source files, result files, executable file if any)'),
    '#prefix' => '<div style="color:red">',
    '#suffix' => '</div>',
  );

  $form['sourcefile'] = array(
    '#type' => 'fieldset',
    '#title' => t('Main or Source Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['sourcefile']['sourcefile1'] = array(
      '#type' => 'file',
      '#title' => t('Upload main or source file'),
      '#size' => 48,
      '#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' .
      t('Allowed file extensions : ') . variable_get('lab_migration_source_extensions', ''),
  );

  $form['dep_files'] = array(
    '#type' => 'item',
    '#title' => t('Dependency Files'),
  );

  /************ START OF EXISTING DEPENDENCIES **************/

  /* existing dependencies */
  $form['existing_depfile'] = array(
    '#type' => 'fieldset',
    '#title' => t('Use Already Existing Dependency Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#prefix' => '<div id="existing-depfile-wrapper">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
  );

  /* existing dependencies */
  $form['existing_depfile']['selected'] = array(
    '#type' => 'item',
    '#title' => t('Existing Dependency Files Selected'),
    '#value' => '<div id="existing_depfile_selected"></div>',
  );

  $form['existing_depfile']['dep_lab_title'] = array(
      '#type' => 'select',
      '#title' => t('Title of the Lab'),
      '#options' => _list_of_lab_titles(),
  );

  list($files_options, $files_options_class) = _list_of_dependency_files();
  $form['existing_depfile']['dep_experiment_files'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Dependency Files'),
      '#options' => $files_options,
      '#options_class' => $files_options_class,
      '#multiple' => TRUE,
  );

  $form['existing_depfile']['dep_upload'] = array(
      '#type' => 'item',
      '#value' => l('Upload New Depedency Files', 'lab_migration/code/upload_dep'),
  );
  /************ END OF EXISTING DEPENDENCIES **************/

  $form['result'] = array(
    '#type' => 'fieldset',
    '#title' => t('Result Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['result']['result1'] = array(
      '#type' => 'file',
      '#title' => t('Upload result file'),
      '#size' => 48,
      '#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' .
      t('Allowed file extensions : ') . variable_get('lab_migration_result_extensions', ''),
  );
  $form['result']['result2'] = array(
      '#type' => 'file',
      '#title' => t('Upload result file'),
      '#size' => 48,
      '#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' .
      t('Allowed file extensions : ') . variable_get('lab_migration_result_extensions', ''),
  );

  $form['xcos'] = array(
    '#type' => 'fieldset',
    '#title' => t('XCOS Files'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
  );
  $form['xcos']['xcos1'] = array(
      '#type' => 'file',
      '#title' => t('Upload xcos file'),
      '#size' => 48,
      '#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' .
      t('Allowed file extensions : ') . variable_get('lab_migration_xcos_extensions', ''),
  );
  $form['xcos']['xcos2'] = array(
      '#type' => 'file',
      '#title' => t('Upload xcos file'),
      '#size' => 48,
      '#description' => t('Separate filenames with underscore. No spaces or any special characters allowed in filename.') . '<br />' .
      t('Allowed file extensions : ') . variable_get('lab_migration_xcos_extensions', ''),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit')
  );

  $form['cancel'] = array(
    '#type' => 'markup',
    '#value' => l(t('Cancel'), 'lab_migration/code'),
  );
  return $form;
}

function upload_code_form_validate($form, &$form_state)
{
  if (!check_code_number($form_state['values']['code_number']))
    form_set_error('code_number', t('Invalid Code Number. Code Number can contain only alphabets and numbers sepereated by dot.'));

  if (!check_name($form_state['values']['code_caption']))
    form_set_error('code_caption', t('Caption can contain only alphabets, numbers and spaces.'));

  if (isset($_FILES['files']))
  {
    /* check if atleast one source or result file is uploaded */  
    if ( ! ($_FILES['files']['name']['sourcefile1'] || $_FILES['files']['name']['xcos1']))
      form_set_error('sourcefile1', t('Please upload atleast one main or source file or xcos file.'));

    /* check for valid filename extensions */  
    foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
    {
      if ($file_name)
      {
        /* checking file type */
        if (strstr($file_form_name, 'source'))
          $file_type = 'S';
        else if (strstr($file_form_name, 'result'))
          $file_type = 'R';
        else if (strstr($file_form_name, 'xcos'))
          $file_type = 'X';
        else
          $file_type = 'U';

        $allowed_extensions_str = '';
        switch ($file_type)
        {
        case 'S':
          $allowed_extensions_str = variable_get('lab_migration_source_extensions', '');
          break;
        case 'R':
          $allowed_extensions_str = variable_get('lab_migration_result_extensions', '');
          break;
        case 'X':
          $allowed_extensions_str = variable_get('lab_migration_xcos_extensions', '');
          break;
        }

        $allowed_extensions = explode(',' , $allowed_extensions_str);
        $temp_extension = end(explode('.', strtolower($_FILES['files']['name'][$file_form_name])));
        if (!in_array($temp_extension, $allowed_extensions))
          form_set_error($file_form_name, t('Only file with ' . $allowed_extensions_str . ' extensions can be uploaded.'));
        if ($_FILES['files']['size'][$file_form_name] <= 0)
          form_set_error($file_form_name, t('File size cannot be zero.'));

        /* check if valid file name */
        if (!lab_migration_check_valid_filename($_FILES['files']['name'][$file_form_name]))
         form_set_error($file_form_name, t('Invalid file name specified. Only alphabets, numbers and underscore is allowed as a valid filename.'));
      }
    }
  }

  /* add javascript again for automatic book title, check if example uploaded, dependency selection effects */
  $dep_selection_js = " $(document).ready(function() {
    $('#edit-existing-depfile-dep-lab-title-wrapper').change(function() {
      var dep_selected = ''; 
      /* showing and hiding relevant files */
      $('.form-checkboxes .option').hide();
      $('.form-checkboxes .option').each(function(index) {
        var activeClass = $('#edit-existing-depfile-dep-book-title').val();
        if ($(this).children().hasClass(activeClass)) {
          $(this).show();
        }
        if ($(this).children().attr('checked') == true) {
          dep_selected += $(this).children().next().text() + '<br />';
        }
      });
      /* showing list of already existing dependencies */
      $('#existing_depfile_selected').html(dep_selected);
    });

    $('.form-checkboxes .option').change(function() {
      $('#edit-existing-depfile-dep-book-title').trigger('change');
    });
    $('#edit-existing-depfile-dep-book-title').trigger('change');
  });";
  drupal_add_js($dep_selection_js, 'inline', 'header');
}

function upload_code_form_submit($form, &$form_state) {
  global $user;

  $root_path = lab_migration_path();

  /************************ check proposal details ************************/
  $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE uid = %d ORDER BY id DESC LIMIT 1", $user->uid);
  $proposal_data = db_fetch_object($proposal_q);
  if (!$proposal_data)
  {
    drupal_set_message("Please submit a " . l('proposal', 'lab_migration/proposal') . ".", 'error');
    drupal_goto('');
  }
  if ($proposal_data->proposal_status != 1)
  {
    switch ($proposal_data->approval_status )
    {
    case 0:
      drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
      drupal_goto('');
      return;
      break;
    case 1:
    	break;
    case 2:
      drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal ' . l('here', 'lab_migration/proposal') . '.'), 'error');
      drupal_goto('');
      return;
      break;
    case 3:
      drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'lab_migration/proposal') . '.'), 'status');
      drupal_goto('');
      return;
      break;
    default:
       drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
       drupal_goto('');
       return;
       break;
    }
  }
  $proposal_id = $proposal_data->id;

  /************************ check experiment details ************************/
  $experiment_id = (int)$form_state['values']['experiment'];
  $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE id = %d AND proposal_id = %d LIMIT 1", $experiment_id, $proposal_id);
  $experiment_data = db_fetch_object($experiment_q);
  if (!$experiment_data)
  {
    drupal_set_message("Invalid experiment seleted", 'error');
    drupal_goto('lab_migration/code');
  }

  /* create proposal folder if not present */
  $dest_path = $proposal_id . '/';
  if (!is_dir($root_path . $dest_path))
    mkdir($root_path . $dest_path);

  /*  get solution details - dont allow if already solution present */
  $cur_solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE experiment_id = %d AND code_number = '%s'", $experiment_id, $form_state['values']['code_number']);
  if ($cur_solution_d = db_fetch_object($cur_solution_q))
  {
    if ($cur_solution_d->approval_status == 1)
    {
      drupal_set_message(t("Solution already approved. Cannot overwrite it."), 'error');
      drupal_goto('lab_migration/code');
      return;
    } else if ($cur_solution_d->approval_status == 0) {
      drupal_set_message(t("Solution is under pending review. Delete the solution and reupload it."), 'error');
      drupal_goto('lab_migration/code');
      return;
    } else {
      drupal_set_message(t("Error uploading solution. Please contact administrator."), 'error');
      drupal_goto('lab_migration/code');
      return;
    }
  }

  /* creating experiment directories */
  $dest_path .= 'EXP' . $experiment_data->number . '/';
  if (!is_dir($root_path . $dest_path))
    mkdir($root_path . $dest_path);

  /* creating code directories */
  $dest_path .= 'CODE' . $form_state['values']['code_number'] . '/';
  if (!is_dir($root_path . $dest_path))
    mkdir($root_path . $dest_path);

  /* creating solution database entry */
  db_query("INSERT INTO {lab_migration_solution} (experiment_id, approver_uid, code_number, caption, approval_date, approval_status, timestamp) VALUES (%d, %d, '%s', '%s', %d, %d, %d)",
    $experiment_id,
    0,
    $form_state['values']['code_number'],
    $form_state['values']['code_caption'],
    0,
    0,
    time()
  );
  $solution_id = db_last_insert_id('lab_migration_solution', 'id');

  /* linking existing dependencies */
  foreach ($form_state['values']['existing_depfile']['dep_experiment_files'] as $row)
  {
    if ($row > 0)
    {
      /* insterting into database */
      db_query("INSERT INTO {lab_migration_solution_dependency} (solution_id, dependency_id)
        VALUES (%d, %d)",
        $solution_id,
        $row
      );
    }
  }

  /* uploading files */
  foreach ($_FILES['files']['name'] as $file_form_name => $file_name)
  {
    if ($file_name)
    {
      /* checking file type */
      if (strstr($file_form_name, 'source'))
        $file_type = 'S';
      else if (strstr($file_form_name, 'result'))
        $file_type = 'R';
      else if (strstr($file_form_name, 'xcos'))
        $file_type = 'X';
      else
        $file_type = 'U';

      if (file_exists($root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
      {
        drupal_set_message(t("Error uploading file. File !filename already exists.", array('!filename' => $_FILES['files']['name'][$file_form_name])), 'error');
        return;
      }

      /* uploading file */
      if (move_uploaded_file($_FILES['files']['tmp_name'][$file_form_name], $root_path . $dest_path . $_FILES['files']['name'][$file_form_name]))
      {
        /* for uploaded files making an entry in the database */
        db_query("INSERT INTO {lab_migration_solution_files} (solution_id, filename, filepath, filemime, filesize, filetype, timestamp)
          VALUES (%d, '%s', '%s', '%s', %d, '%s', %d)",
          $solution_id,
          $_FILES['files']['name'][$file_form_name],
          $dest_path . $_FILES['files']['name'][$file_form_name],
          $_FILES['files']['type'][$file_form_name],
          $_FILES['files']['size'][$file_form_name],
          $file_type,
          time()
          );
        drupal_set_message($file_name . ' uploaded successfully.', 'status');
      } else {
        drupal_set_message('Error uploading file : ' . $dest_path . '/' . $file_name, 'error');
      }
    }
  }
  drupal_set_message('Example uploaded successfully.', 'status');

  /* sending email */
  $email_to = $user->mail . ', ' . variable_get('lab_migration_emails', '');
  $param['solution_uploaded']['solution_id'] = $example_id;
  $param['solution_uploaded']['user_id'] = $user->uid;
  if (!drupal_mail('lab_migration', 'solution_uploaded', $email_to, language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
    drupal_set_message('Error sending email message.', 'error');

  drupal_goto('lab_migration/code');
}

/******************************************************************************/
/************************** GENERAL FUNCTIONS *********************************/
/******************************************************************************/

function _list_of_lab_titles()
{
  $lab_titles = array('0' => 'Please select...');
  $lab_titles_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE approval_status = 1 OR approval_status = 3 ORDER BY lab_title ASC");
  while ($lab_titles_data = db_fetch_object($lab_titles_q))
  {
    $lab_titles[$lab_titles_data->id] = $lab_titles_data->lab_title . ' (Proposed by ' . $lab_titles_data->name . ')';
  }
  return $lab_titles;
}

function _list_of_dependency_files()
{
  $book_dependency_files = array();
  $book_dependency_files_class = array();
  $book_dependency_files_q = db_query("SELECT * FROM {lab_migration_dependency_files} ORDER BY filename ASC");

  while ($book_dependency_files_data = db_fetch_object($book_dependency_files_q))
  {
    $temp_caption = '';
    if ($book_dependency_files_data->caption)
      $temp_caption .= ' (' . $book_dependency_files_data->caption . ')';
    $book_dependency_files[$book_dependency_files_data->id] = l($book_dependency_files_data->filename . $temp_caption, 'download/dependency/' . $book_dependency_files_data->id);
    $book_dependency_files_class[$book_dependency_files_data->id] = $book_dependency_files_data->preference_id;
  }
  return array($book_dependency_files, $book_dependency_files_class);
}

