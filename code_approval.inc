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


/******************************************************************************/
/********************************* BULK APPROVAL ******************************/
/******************************************************************************/

function bulk_approval_form($form_state)
{
  $form['#redirect'] = FALSE;

  ahah_helper_register($form, $form_state);

  /* default value for ahah fields */
  if (!isset($form_state['storage']['run']['book']))
  {
    $book_default_value = 0;
  } else {
    $book_default_value =  $form_state['storage']['run']['book'];
  }

  if (!isset($form_state['storage']['run']['chapter']))
  {
    $chapter_default_value = 0;
  } else {
    if ($form_state['values']['run']['book_hidden'] !=  $form_state['values']['run']['book'])
      $chapter_default_value = 0;
    else
      $chapter_default_value = $form_state['storage']['run']['chapter'];
  }

  if (!isset($form_state['storage']['run']['example']))
  {
    $example_default_value = 0;
  } else {
    if ($form_state['values']['run']['book_hidden'] !=  $form_state['values']['run']['book'])
      $example_default_value = 0;
    else if ($form_state['values']['run']['chapter_hidden'] !=  $form_state['values']['run']['chapter'])
      $example_default_value = 0;
    else
      $example_default_value = $form_state['storage']['run']['example'];
  }

  $form['run'] = array(
    '#type' => 'fieldset',
    '#title' => t('Bulk Manage Code'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE,
    '#prefix' => '<div id="run-wrapper">',
    '#suffix' => '</div>',
    '#tree' => TRUE,
  );

  $form['run']['book'] = array(
      '#type' => 'select',
      '#title' => t('Title of the Book'),
      '#options' => _list_of_books(),
      '#default_value' => $book_default_value,
      '#tree' => TRUE,
      '#ahah' => array(
        'event'   => 'change',
        'effect' => 'none',
        'path'    => ahah_helper_path(array('run')),
        'wrapper' => 'run-wrapper',
        'progress' => array(
          'type' => 'throbber',
          'message' => t(''),
        ),
      ),
  );

  /* hidden form elements */
  $form['run']['book_hidden'] = array(
    '#type'  => 'hidden',
    '#value' =>  $form_state['values']['run']['book'],
  );

  /* hidden form elements */
  $form['run']['chapter_hidden'] = array(
    '#type'  => 'hidden',
    '#value' =>  $form_state['values']['run']['chapter'],
  );

  if ($book_default_value > 0)
  {
    $form['run']['download_book'] = array(
      '#type' => 'item',
      '#value' => l('Download', 'full_download/book/' . $book_default_value) . ' ' . t('(Download all the approved and unapproved examples of the entire book)'),
    );
    $form['run']['download_pdf'] = array(
      '#type' => 'item',
      '#value' => l('Download PDF', 'lab_migration/generate_book/' . $book_default_value . '/1') . ' ' . t('(Download PDF of all the approved and unapproved examples of the entire book)'),
    );
    $form['run']['regenrate_book'] = array(
      '#type' => 'item',
      '#value' => l('Regenerate PDF', 'lab_migration/delete_book/' . $book_default_value) . ' ' . t('(Manually Regenerate PDF of the entire book)'),
    );
    $form['run']['notes_book'] = array(
      '#type' => 'item',
      '#value' => l('Notes for Reviewers', 'code_approval/notes/' . $book_default_value),
    );

    $form['run']['approve_book'] = array(
      '#type'  => 'checkbox',
      '#title' => t('Approve Entire Book'),
    );
    $form['run']['unapprove_book'] = array(
      '#type'  => 'checkbox',
      '#title' => t('Pending Review Entire Book'),
    );
    $form['run']['disapprove_book'] = array(
      '#type'  => 'checkbox',
      '#title' => t('Dis-Approve Entire Book (This will delete all the examples in the book)'),
      '#prefix' => '<div style="color:red;"><strong>',
      '#suffix' => '</strong></div>',
    );
    $form['run']['delete_book_including_proposal'] = array(
      '#type'  => 'checkbox',
      '#title' => t('Delete Entire Book Including Proposal'),
      '#prefix' => '<div style="color:red;"><strong>',
      '#suffix' => '</strong></div>',
    );

    $form['run']['chapter'] = array(
      '#type' => 'select',
      '#title' => t('Title of the Chapter'),
      '#options' => _list_of_chapters($book_default_value),
      '#default_value' => $chapter_default_value,
      '#tree' => TRUE,
      '#ahah' => array(
        'event'   => 'change',
        'effect' => 'none',
        'path'    => ahah_helper_path(array('run')),
        'wrapper' => 'run-wrapper',
        'progress' => array(
          'type' => 'throbber',
          'message' => t(''),
        ),
      ),
    );
    if ($chapter_default_value > 0)
    {
      $form['run']['download_chapter'] = array(
        '#type' => 'item',
        '#value' => l('Download', 'full_download/chapter/' . $chapter_default_value) . ' ' . t('(Download all the approved and unapproved examples of the entire chapter)'),
      );

      $form['run']['approve_chapter'] = array(
        '#type'  => 'checkbox',
        '#title' => t('Approve Entire Chapter'),
      );
      $form['run']['unapprove_chapter'] = array(
        '#type'  => 'checkbox',
        '#title' => t('Pending Review Entire Chapter'),
      );
      $form['run']['disapprove_chapter'] = array(
        '#type'  => 'checkbox',
        '#title' => t('Dis-Approve Entire Chapter (This will delete all the examples in the chapter)'),
        '#prefix' => '<div style="color:red;"><strong>',
        '#suffix' => '</strong></div>',
      );

      $form['run']['example'] = array(
        '#type' => 'select',
        '#title' => t('Example No. (Caption)'),
        '#options' => _list_of_examples($chapter_default_value),
        '#default_value' => $example_default_value,
        '#tree' => TRUE,
        '#ahah' => array(
          'event'   => 'change',
          'effect' => 'none',
          'path'    => ahah_helper_path(array('run')),
          'wrapper' => 'run-wrapper',
          'progress' => array(
            'type' => 'throbber',
            'message' => t(''),
          ),
        ),
      );
    }
  }

  /************ START OF $_POST **************/
  if ($_POST)
  {
    if (($book_default_value > 0) && ($chapter_default_value > 0) && ($example_default_value > 0))
    {
      $example_list_q = db_query("SELECT * FROM {lab_migration_example_files} WHERE example_id = %d", $form_state['values']['run']['example']);
      if ($example_list_q)
      {
        $example_files_rows = array();
        while ($example_list_data = db_fetch_object($example_list_q))
        {
          $example_file_type = '';
          switch ($example_list_data->filetype)
          {
          case 'S' : $example_file_type = 'Source or Main file'; break;
          case 'R' : $example_file_type = 'Result file'; break;
          case 'X' : $example_file_type = 'xcos file'; break;
          default  : $example_file_type = 'Unknown'; break;
          }
          $example_files_rows[] = array(l($example_list_data->filename, 'download/file/' . $example_list_data->id), $example_file_type);
        }

        /* dependency files */
        $dependency_list_q = db_query("SELECT dependency.id as dependency_id, dependency.filename as dependency_filename, dependency.caption as dependency_caption
          FROM {lab_migration_example_dependency} example_dependency LEFT JOIN {lab_migration_dependency_files} dependency
          ON example_dependency.dependency_id = dependency.id
          WHERE example_dependency.example_id = %d", $form_state['values']['run']['example']);
          while ($dependency_list_data = db_fetch_object($dependency_list_q))
          {
            $example_file_type = 'Dependency file';
            $temp_caption = '';
            if ($dependency_list_data->dependency_caption)
              $temp_caption = ' (' . $dependency_list_data->dependency_caption . ')';
            $example_files_rows[] = array(l($dependency_list_data->dependency_filename, 'download/dependency/' . $dependency_list_data->dependency_id) . $temp_caption, $example_file_type);
          }

        /* creating list of files table */
        $example_files_header = array('Filename', 'Type');
        $example_files = theme_table($example_files_header, $example_files_rows); 
      }
      $form['run']['download_example'] = array(
        '#type' => 'item',
        '#value' => l('Download Example', 'download/example/' . $example_default_value),
      );
      $form['run']['edit_example'] = array(
        '#type'  => 'item',
        '#value' => l('Edit Example', 'code_approval/editcode/' . $example_default_value),
      );

      $form['run']['example_files'] = array(
        '#type'  => 'item',
        '#title' => 'List of example files',
        '#value' => $example_files,
      );

      $form['run']['approve_example'] = array(
        '#type'  => 'checkbox',
        '#title' => t('Approve Example'),
      );
      $form['run']['unapprove_example'] = array(
        '#type'  => 'checkbox',
        '#title' => t('Pending Review Example'),
      );
      $form['run']['disapprove_example'] = array(
        '#type'  => 'checkbox',
        '#title' => t('Dis-approve Example (This will delete the example)'),
        '#prefix' => '<div style="color:red;"><strong>',
        '#suffix' => '</strong></div>',
      );
    }
  }
  /************ END OF $_POST **************/

  if ($book_default_value > 0)
  {
    $form['run']['message'] = array(
      '#type' => 'textarea',
      '#title' => t('If Dis-Approved please specify reason for Dis-Approval'),
    );
  
    $form['run']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit')
    );
  }

  return $form;
}


function bulk_approval_form_submit($form, &$form_state)
{
	global $user;
  $root_path = lab_migration_path();

	if ($form_state['clicked_button']['#value'] == 'Submit')
	{
		if ($form_state['values']['run']['book'])
			del_book_pdf($form_state['values']['run']['book']);

	  if (user_access('bulk manage code'))
    {
      if ($form_state['values']['run']['approve_book'] == "1")
      {

        /* approving entire book */
        $chapter_q = db_query("SELECT * FROM {lab_migration_chapter} WHERE preference_id = %d", $form_state['values']['run']['book']);
        while ($chapter_data = db_fetch_object($chapter_q))
        {
          db_query("UPDATE {lab_migration_example} SET approval_status = 1, approver_uid = %d WHERE chapter_id = %d AND approval_status = 0", $user->uid, $chapter_data->id);
        }
        drupal_set_message(t('Approved Entire Book.'), 'status');

        /* email */
        $email_subject = t('Your uploaded examples have been approved');
        $email_body = t('Your all the uploaded examples for the book have been approved.');

      } else if ($form_state['values']['run']['unapprove_book'] == "1") {

        /* approving entire book */
        $chapter_q = db_query("SELECT * FROM {lab_migration_chapter} WHERE preference_id = %d", $form_state['values']['run']['book']);
        while ($chapter_data = db_fetch_object($chapter_q))
        {
          db_query("UPDATE {lab_migration_example} SET approval_status = 0 WHERE chapter_id = %d", $chapter_data->id);
        }
        drupal_set_message(t('Pending Review Entire Book.'), 'status');

        /* email */
        $email_subject = t('Your uploaded examples have been marked as pending');
        $email_body = t('Your all the uploaded examples for the book have been marked as pending to be review. You will be able to see the exmaples after they have been approved by one of our reviewers.');

      } else if ($form_state['values']['run']['disapprove_book'] == "1") {

        if (!user_access('bulk delete code'))
        {
          drupal_set_message(t('You do not have permission to Bulk Dis-Approved and Deleted Entire Book.'), 'error');
          return;
        }

        if (delete_book($form_state['values']['run']['book']))
        {
          drupal_set_message(t('Dis-Approved and Deleted Entire Book.'), 'status');
        } else {
          drupal_set_message(t('Error Dis-Approving and Deleting Entire Book.'), 'error');
        }

        /* email */
        $email_subject = t('Your uploaded examples have been marked as dis-approved');
        $email_body = t('Your all the uploaded examples for the whole book have been marked as dis-approved.

Reason for dis-approval:

' . $form_state['values']['run']['message']);

      } else if ($form_state['values']['run']['delete_book_including_proposal'] == "1") {

        if (!user_access('bulk delete code'))
        {
          drupal_set_message(t('You do not have permission to Bulk Delete Entire Book Including Proposal.'), 'error');
          return;
        }

        /* check if dependency files are present */
        $dep_q = db_query("SELECT * FROM {lab_migration_dependency_files} WHERE preference_id = %d", $form_state['values']['run']['book']);
        if ($dep_data = db_fetch_object($dep_q))
        {
          drupal_set_message(t("Cannot delete book since it has dependency files that can be used by others. First delete the dependency files before deleing the Book."), 'error');
          return;
        }

        if (delete_book($form_state['values']['run']['book']))
        {
          drupal_set_message(t('Dis-Approved and Deleted Entire Book examples.'), 'status');

          $dir_path = $root_path . $form_state['values']['run']['book'];
          if (is_dir($dir_path))
          {
            $res = rmdir($dir_path);
            if (!$res)
            {
              drupal_set_message(t("Cannot delete Book directory : " . $dir_path . ". Please contact administrator."), 'error');
              return;
            }
          } else {
            drupal_set_message(t("Book directory not present : " . $dir_path . ". Skipping deleting book directory."), 'status');
          }

          /* deleting preference and proposal */
          $preference_q = db_query("SELECT * FROM {lab_migration_preference} WHERE id = %d", $form_state['values']['run']['book']);
          $preference_data = db_fetch_object($preference_q);
          $proposal_id = $preference_data->proposal_id;
          db_query("DELETE FROM {lab_migration_preference} WHERE proposal_id = %d", $proposal_id);
          db_query("DELETE FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);

          drupal_set_message(t('Deleted Book Proposal.'), 'status');

          /* email */
          $email_subject = t('Your uploaded examples including the book proposal have been deleted');
          $email_body = t('Your all the uploaded examples including the book have been deleted permanently.');

        } else {
          drupal_set_message(t('Error Dis-Approving and Deleting Entire Book.'), 'error');
        }

      } else if ($form_state['values']['run']['approve_chapter'] == "1") {

        db_query("UPDATE {lab_migration_example} SET approval_status = 1, approver_uid = %d WHERE chapter_id = %d AND approval_status = 0", $user->uid, $form_state['values']['run']['chapter']);
        drupal_set_message(t('Approved Entire Chapter.'), 'status');

        /* email */
        $email_subject = t('Your uploaded examples have been approved');
        $email_body = t('Your all the uploaded examples for the chapter have been approved.');

      } else if ($form_state['values']['run']['unapprove_chapter'] == "1") {

        db_query("UPDATE {lab_migration_example} SET approval_status = 0 WHERE chapter_id = %d", $form_state['values']['run']['chapter']);
        drupal_set_message(t('Entire Chapter marked as Pending Review.'), 'status');

        /* email */
        $email_subject = t('Your uploaded examples have been marked as pending');
        $email_body = t('Your all the uploaded examples for the chapter have been marked as pending to be review.');

      } else if ($form_state['values']['run']['disapprove_chapter'] == "1") {

        if (!user_access('bulk delete code'))
        {
          drupal_set_message(t('You do not have permission to Bulk Dis-Approved and Deleted Entire Chapter.'), 'error');
          return;
        }

        if (delete_chapter($form_state['values']['run']['chapter']))
        {
          drupal_set_message(t('Dis-Approved and Deleted Entire Chapter.'), 'status');
        } else {
          drupal_set_message(t('Error Dis-Approving and Deleting Entire Chapter.'), 'error');
        }

        /* email */
        $email_subject = t('Your uploaded example have been marked as dis-approved');
        $email_body = t('Your uploaded example for the entire chapter have been marked as dis-approved.

Reason for dis-approval:

' . $form_state['values']['run']['message']);

      } else if ($form_state['values']['run']['approve_example'] == "1") {

        db_query("UPDATE {lab_migration_example} SET approval_status = 1, approver_uid = %d WHERE id = %d", $user->uid, $form_state['values']['run']['example']);
        drupal_set_message(t('Example approved.'), 'status');

        /* email */
        $email_subject = t('Your uploaded example has been approved');
        $email_body = t('Your uploaded example has been approved.');

      } else if ($form_state['values']['run']['unapprove_example'] == "1") {

        db_query("UPDATE {lab_migration_example} SET approval_status = 0 WHERE id = %d", $form_state['values']['run']['example']);
        drupal_set_message(t('Example marked as Pending Review.'), 'status');

        /* email */
        $email_subject = t('Your uploaded example has been marked as pending');
        $email_body = t('Your uploaded example has been marked as pending to be review.');

      } else if ($form_state['values']['run']['disapprove_example'] == "1") {

        if (delete_example($form_state['values']['run']['example']))
        {
          drupal_set_message(t('Example Dis-Approved and Deleted.'), 'status');
        } else {
          drupal_set_message(t('Error Dis-Approving and Deleting Example.'), 'error');
        }

        /* email */
        $email_subject = t('Your uploaded example has been marked as dis-approved');
        $email_body = t('Your uploaded example has been marked as dis-approved.

Reason for dis-approval:

' . $form_state['values']['run']['message']);

      }

      /****** sending email when everything done ******/
      if ($email_subject)
      {
        $email_to = variable_get('lab_migration_emails', '') . ', ' . $user->mail;
        $param['standard']['subject'] = $email_subject;
        $param['standard']['body'] = $email_body;
        if (!drupal_mail('lab_migration', 'standard', $email_to, language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
          drupal_set_message('Error sending email message.', 'error');
      }

    } else {
      drupal_set_message(t('You do not have permission to bulk manage code.'), 'error');
    }
	}
}

function _list_of_books()
{
  $book_titles = array('0' => 'Please select...');
  $book_titles_q = db_query("SELECT * FROM {lab_migration_preference} WHERE approval_status = 1 OR approval_status = 3 ORDER BY book ASC");
  while ($book_titles_data = db_fetch_object($book_titles_q))
  {
    $book_titles[$book_titles_data->id] = $book_titles_data->book . ' (Written by ' . $book_titles_data->author . ')';
  }
  return $book_titles;
}

function _list_of_chapters($preference_id = 0)
{
  $book_chapters = array('0' => 'Please select...');
  $book_chapters_q = db_query("SELECT * FROM {lab_migration_chapter} WHERE preference_id = %d ORDER BY number ASC", $preference_id);
  while ($book_chapters_data = db_fetch_object($book_chapters_q))
  {
    $book_chapters[$book_chapters_data->id] = $book_chapters_data->number . '. ' . $book_chapters_data->name;
  }
  return $book_chapters;
}

function _list_of_examples($chapter_id = 0)
{
  $book_examples = array('0' => 'Please select...');
  $book_examples_q = db_query("SELECT * FROM {lab_migration_example} WHERE chapter_id = %d ORDER BY
    CAST(SUBSTRING_INDEX(number, '.', 1) AS BINARY) ASC,
    CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(number , '.', 2), '.', -1) AS UNSIGNED) ASC,
    CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(number , '.', -1), '.', 1) AS UNSIGNED) ASC", $chapter_id);
  while ($book_examples_data = db_fetch_object($book_examples_q))
  {
    $book_examples[$book_examples_data->id] = $book_examples_data->number . ' (' . $book_examples_data->caption . ')';
  }
  return $book_examples;
}

