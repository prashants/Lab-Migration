<?php

/******************************************************************************/
/********************************* BULK APPROVAL ******************************/
/******************************************************************************/

function lab_migration_bulk_approval_form($form_state)
{
  $form['#redirect'] = FALSE;

  ahah_helper_register($form, $form_state);

  /* default value for ahah fields */
  if (!isset($form_state['storage']['run']['lab']))
  {
    $lab_default_value = 0;
  } else {
    $lab_default_value =  $form_state['storage']['run']['lab'];
  }

  if (!isset($form_state['storage']['run']['experiment']))
  {
    $experiment_default_value = 0;
  } else {
    if ($form_state['values']['run']['lab_hidden'] !=  $form_state['values']['run']['lab'])
      $experiment_default_value = 0;
    else
      $experiment_default_value = $form_state['storage']['run']['experiment'];
  }

  if (!isset($form_state['storage']['run']['solution']))
  {
    $solution_default_value = 0;
  } else {
    if ($form_state['values']['run']['lab_hidden'] !=  $form_state['values']['run']['lab'])
      $solution_default_value = 0;
    else if ($form_state['values']['run']['experiment_hidden'] !=  $form_state['values']['run']['experiment'])
      $solution_default_value = 0;
    else
      $solution_default_value = $form_state['storage']['run']['solution'];
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

  $form['run']['lab'] = array(
      '#type' => 'select',
      '#title' => t('Title of the Lab'),
      '#options' => _list_of_labs(),
      '#default_value' => $lab_default_value,
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
  $form['run']['lab_hidden'] = array(
    '#type'  => 'hidden',
    '#value' =>  $form_state['values']['run']['lab'],
  );

  /* hidden form elements */
  $form['run']['experiment_hidden'] = array(
    '#type'  => 'hidden',
    '#value' =>  $form_state['values']['run']['experiment'],
  );

  if ($lab_default_value > 0)
  {
    $form['run']['download_lab'] = array(
      '#type' => 'item',
      '#value' => l('Download', 'lab_migration/full_download/lab/' . $lab_default_value) . ' ' . t('(Download all the approved and unapproved solutions of the entire lab)'),
    );
    $form['run']['download_pdf'] = array(
      '#type' => 'item',
      '#value' => l('Download PDF', 'lab_migration/generate_lab/' . $lab_default_value . '/1') . ' ' . t('(Download PDF of all the approved and unapproved solution of the entire lab)'),
    );
    $form['run']['regenrate_lab'] = array(
      '#type' => 'item',
      '#value' => l('Regenerate PDF', 'lab_migration/delete_lab/' . $lab_default_value) . ' ' . t('(Manually Regenerate PDF of the entire lab)'),
    );
    $form['run']['notes_lab'] = array(
      '#type' => 'item',
      '#value' => l('Notes for Reviewers', '/lab_migration/code_approval/notes/' . $lab_default_value),
    );
    $form['run']['upload_code'] = array(
      '#type' => 'item',
      '#value' => l('Upload Code', '/lab_migration/code_approval/upload/' . $lab_default_value),
    );

    $form['run']['approve_lab'] = array(
      '#type'  => 'checkbox',
      '#title' => t('Approve Entire Lab'),
    );
    $form['run']['unapprove_lab'] = array(
      '#type'  => 'checkbox',
      '#title' => t('Pending Review Entire Lab'),
    );
    $form['run']['disapprove_lab'] = array(
      '#type'  => 'checkbox',
      '#title' => t('Dis-Approve Entire Lab (This will delete all the solutions in the lab)'),
      '#prefix' => '<div style="color:red;"><strong>',
      '#suffix' => '</strong></div>',
    );
    $form['run']['delete_lab_including_proposal'] = array(
      '#type'  => 'checkbox',
      '#title' => t('Delete Entire Lab Including Proposal'),
      '#prefix' => '<div style="color:red;"><strong>',
      '#suffix' => '</strong></div>',
    );

    $form['run']['experiment'] = array(
      '#type' => 'select',
      '#title' => t('Title of the Experiment'),
      '#options' => _list_of_experiments($lab_default_value),
      '#default_value' => $experiment_default_value,
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
    if ($experiment_default_value > 0)
    {
      $form['run']['download_experiment'] = array(
        '#type' => 'item',
        '#value' => l('Download', 'lab_migration/full_download/experiment/' . $experiment_default_value) . ' ' . t('(Download all the approved and unapproved solutions of the experiment)'),
      );

      $form['run']['approve_experiment'] = array(
        '#type'  => 'checkbox',
        '#title' => t('Approve Entire Experiment'),
      );
      $form['run']['unapprove_experiment'] = array(
        '#type'  => 'checkbox',
        '#title' => t('Pending Review Entire Experiment'),
      );
      $form['run']['disapprove_experiment'] = array(
        '#type'  => 'checkbox',
        '#title' => t('Dis-Approve Entire Experiment (This will delete all the solutions in the experiment)'),
        '#prefix' => '<div style="color:red;"><strong>',
        '#suffix' => '</strong></div>',
      );

      $form['run']['solution'] = array(
        '#type' => 'select',
        '#title' => t('Solution No. (Caption)'),
        '#options' => _list_of_solutions($experiment_default_value),
        '#default_value' => $solution_default_value,
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
    if (($lab_default_value > 0) && ($experiment_default_value > 0) && ($solution_default_value > 0))
    {
      $solution_list_q = db_query("SELECT * FROM {lab_migration_solution_files} WHERE solution_id = %d", $form_state['values']['run']['solution']);
      if ($solution_list_q)
      {
        $solution_files_rows = array();
        while ($solution_list_data = db_fetch_object($solution_list_q))
        {
          $solution_file_type = '';
          switch ($solution_list_data->filetype)
          {
          case 'S' : $solution_file_type = 'Source or Main file'; break;
          case 'R' : $solution_file_type = 'Result file'; break;
          case 'X' : $solution_file_type = 'xcos file'; break;
          default  : $solution_file_type = 'Unknown'; break;
          }
          $solution_files_rows[] = array(l($solution_list_data->filename, 'lab_migration/download/file/' . $solution_list_data->id), $solution_file_type);
        }

        /* dependency files */
        $dependency_q = db_query("SELECT * FROM {lab_migration_solution_dependency} WHERE solution_id = %d", $form_state['values']['run']['solution']);
				while ($dependency_data = db_fetch_object($dependency_q))
				{
					$dependency_files_q = db_query("SELECT * FROM {lab_migration_dependency_files} WHERE id = %d", $dependency_data->dependency_id);
					$dependency_files_data = db_fetch_object($dependency_files_q);
					$solution_file_type = 'Dependency file';
					$solution_files_rows[] = array(l($dependency_files_data->filename, 'lab_migration/download/dependency/' . $dependency_files_data->dependency_id), $solution_file_type);
				}

        /* creating list of files table */
        $solution_files_header = array('Filename', 'Type');
        $solution_files = theme_table($solution_files_header, $solution_files_rows); 
      }
      $form['run']['download_solution'] = array(
        '#type' => 'item',
        '#value' => l('Download Solution', 'lab_migration/download/solution/' . $solution_default_value),
      );
      $form['run']['edit_solution'] = array(
        '#type'  => 'item',
        '#value' => l('Edit Solution', 'code_approval/editcode/' . $solution_default_value),
      );

      $form['run']['solution_files'] = array(
        '#type'  => 'item',
        '#title' => 'List of solution files',
        '#value' => $solution_files,
      );

      $form['run']['approve_solution'] = array(
        '#type'  => 'checkbox',
        '#title' => t('Approve Solution'),
      );
      $form['run']['unapprove_solution'] = array(
        '#type'  => 'checkbox',
        '#title' => t('Pending Review Solution'),
      );
      $form['run']['disapprove_solution'] = array(
        '#type'  => 'checkbox',
        '#title' => t('Dis-approve Solution (This will delete the solution)'),
        '#prefix' => '<div style="color:red;"><strong>',
        '#suffix' => '</strong></div>',
      );
    }
  }
  /************ END OF $_POST **************/

  if ($lab_default_value > 0)
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


function lab_migration_bulk_approval_form_submit($form, &$form_state)
{
	global $user;
  $root_path = lab_migration_path();

	if ($form_state['clicked_button']['#value'] == 'Submit')
	{
		if ($form_state['values']['run']['lab'])
			del_lab_pdf($form_state['values']['run']['lab']);

	  if (user_access('lab migration bulk manage code'))
    {
      if ($form_state['values']['run']['approve_lab'] == "1")
      {

        /* approving entire lab */
        $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d", $form_state['values']['run']['lab']);
        while ($experiment_data = db_fetch_object($experiment_q))
        {
          db_query("UPDATE {lab_migration_solution} SET approval_status = 1, approver_uid = %d WHERE experiment_id = %d AND approval_status = 0", $user->uid, $experiment_data->id);
        }
        drupal_set_message(t('Approved Entire Lab.'), 'status');

        /* email */
        $email_subject = t('Your uploaded solutions have been approved');
        $email_body = t('Your all the uploaded solutions for the Lab have been approved.');

      } else if ($form_state['values']['run']['unapprove_lab'] == "1") {

        /* pending review entire lab */
        $experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d", $form_state['values']['run']['lab']);
        while ($experiment_data = db_fetch_object($experiment_q))
        {
          db_query("UPDATE {lab_migration_solution} SET approval_status = 0 WHERE experiment_id = %d", $experiment_data->id);
        }
        drupal_set_message(t('Pending Review Entire Lab.'), 'status');

        /* email */
        $email_subject = t('Your uploaded solutions have been marked as pending');
        $email_body = t('Your all the uploaded solutions for the Lab have been marked as pending to be review. You will be able to see the solutions after they have been approved by one of our reviewers.');

      } else if ($form_state['values']['run']['disapprove_lab'] == "1") {

        if (!user_access('lab migration bulk delete code'))
        {
          drupal_set_message(t('You do not have permission to Bulk Dis-Approved and Deleted Entire Lab.'), 'error');
          return;
        }

        if (delete_lab($form_state['values']['run']['lab']))
        {
          drupal_set_message(t('Dis-Approved and Deleted Entire Lab.'), 'status');
        } else {
          drupal_set_message(t('Error Dis-Approving and Deleting Entire Lab.'), 'error');
        }

        /* email */
        $email_subject = t('Your uploaded solutions have been marked as dis-approved');
        $email_body = t('Your all the uploaded solutions for the whole Lab have been marked as dis-approved.

Reason for dis-approval:

' . $form_state['values']['run']['message']);

      } else if ($form_state['values']['run']['delete_lab_including_proposal'] == "1") {

        if (!user_access('lab migration bulk delete code'))
        {
          drupal_set_message(t('You do not have permission to Bulk Delete Entire Lab Including Proposal.'), 'error');
          return;
        }

        /* check if dependency files are present */
        $dep_q = db_query("SELECT * FROM {lab_migration_dependency_files} WHERE proposal_id = %d", $form_state['values']['run']['lab']);
        if ($dep_data = db_fetch_object($dep_q))
        {
          drupal_set_message(t("Cannot delete lab since it has dependency files that can be used by others. First delete the dependency files before deleting the lab."), 'error');
          return;
        }

        if (delete_lab($form_state['values']['run']['lab']))
        {
          drupal_set_message(t('Dis-Approved and Deleted Entire Lab solutions.'), 'status');

          $dir_path = $root_path . $form_state['values']['run']['lab'];
          if (is_dir($dir_path))
          {
            $res = rmdir($dir_path);
            if (!$res)
            {
              drupal_set_message(t("Cannot delete Lab directory : " . $dir_path . ". Please contact administrator."), 'error');
              return;
            }
          } else {
            drupal_set_message(t("Lab directory not present : " . $dir_path . ". Skipping deleting lab directory."), 'status');
          }

          /* deleting full proposal */
          $proposal_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $form_state['values']['run']['lab']);
          $proposal_data = db_fetch_object($proposal_q);
          $proposal_id = $proposal_data->id;
          db_query("DELETE FROM {lab_migration_experiment} WHERE proposal_id = %d", $proposal_id);
          db_query("DELETE FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);

          drupal_set_message(t('Deleted Lab Proposal.'), 'status');

          /* email */
          $email_subject = t('Your uploaded solutions including the Lab proposal have been deleted');
          $email_body = t('Your all the uploaded solutions including the Lab proposal have been deleted permanently.');

        } else {
          drupal_set_message(t('Error Dis-Approving and Deleting Entire Lab.'), 'error');
        }

      } else if ($form_state['values']['run']['approve_experiment'] == "1") {

        db_query("UPDATE {lab_migration_solution} SET approval_status = 1, approver_uid = %d WHERE experiment_id = %d AND approval_status = 0", $user->uid, $form_state['values']['run']['experiment']);
        drupal_set_message(t('Approved Entire Experiment.'), 'status');

        /* email */
        $email_subject = t('Your uploaded solutions have been approved');
        $email_body = t('Your all the uploaded solutions for the experiment have been approved.');

      } else if ($form_state['values']['run']['unapprove_experiment'] == "1") {

        db_query("UPDATE {lab_migration_solution} SET approval_status = 0 WHERE experiment_id = %d", $form_state['values']['run']['experiment']);
        drupal_set_message(t('Entire Experiment marked as Pending Review.'), 'status');

        /* email */
        $email_subject = t('Your uploaded solutions have been marked as pending');
        $email_body = t('Your all the uploaded solutions for the experiment have been marked as pending to be review.');

      } else if ($form_state['values']['run']['disapprove_experiment'] == "1") {

        if (!user_access('lab migration bulk delete code'))
        {
          drupal_set_message(t('You do not have permission to Bulk Dis-Approved and Deleted Entire Experiment.'), 'error');
          return;
        }

        if (delete_experiment($form_state['values']['run']['experiment']))
        {
          drupal_set_message(t('Dis-Approved and Deleted Entire Experiment.'), 'status');
        } else {
          drupal_set_message(t('Error Dis-Approving and Deleting Entire Experiment.'), 'error');
        }

        /* email */
        $email_subject = t('Your uploaded solutions have been marked as dis-approved');
        $email_body = t('Your uploaded solutions for the entire experiment have been marked as dis-approved.

Reason for dis-approval:

' . $form_state['values']['run']['message']);

      } else if ($form_state['values']['run']['approve_solution'] == "1") {

        db_query("UPDATE {lab_migration_solution} SET approval_status = 1, approver_uid = %d WHERE id = %d", $user->uid, $form_state['values']['run']['solution']);
        drupal_set_message(t('Solution approved.'), 'status');

        /* email */
        $email_subject = t('Your uploaded solution has been approved');
        $email_body = t('Your uploaded solution has been approved.');

      } else if ($form_state['values']['run']['unapprove_solution'] == "1") {

        db_query("UPDATE {lab_migration_solution} SET approval_status = 0 WHERE id = %d", $form_state['values']['run']['solution']);
        drupal_set_message(t('Solution marked as Pending Review.'), 'status');

        /* email */
        $email_subject = t('Your uploaded solution has been marked as pending');
        $email_body = t('Your uploaded solution has been marked as pending to be review.');

      } else if ($form_state['values']['run']['disapprove_solution'] == "1") {

        if (delete_solution($form_state['values']['run']['solution']))
        {
          drupal_set_message(t('Solution Dis-Approved and Deleted.'), 'status');
        } else {
          drupal_set_message(t('Error Dis-Approving and Deleting Solution.'), 'error');
        }

        /* email */
        $email_subject = t('Your uploaded solution has been marked as dis-approved');
        $email_body = t('Your uploaded solution has been marked as dis-approved.

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

function _list_of_labs()
{
  $lab_titles = array('0' => 'Please select...');
  $lab_titles_q = db_query("SELECT * FROM {lab_migration_proposal} ORDER BY lab_title ASC");
  while ($lab_titles_data = db_fetch_object($lab_titles_q))
  {
    $lab_titles[$lab_titles_data->id] = $lab_titles_data->lab_title . ' (Proposed by ' . $lab_titles_data->name . ')';
  }
  return $lab_titles;
}

function _list_of_experiments($proposal_id = 0)
{
  $experiments = array('0' => 'Please select...');
  $experiments_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE proposal_id = %d ORDER BY number ASC", $proposal_id);
  while ($experiments_data = db_fetch_object($experiments_q))
  {
    $experiments[$experiments_data->id] = $experiments_data->number . '. ' . $experiments_data->title;
  }
  return $experiments;
}

function _list_of_solutions($experiment_id = 0)
{
  $solutions = array('0' => 'Please select...');
  $solutions_q = db_query("SELECT * FROM {lab_migration_solution} WHERE experiment_id = %d ORDER BY
    CAST(SUBSTRING_INDEX(code_number, '.', 1) AS BINARY) ASC,
    CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(code_number , '.', 2), '.', -1) AS UNSIGNED) ASC,
    CAST(SUBSTRING_INDEX(SUBSTRING_INDEX(code_number , '.', -1), '.', 1) AS UNSIGNED) ASC", $experiment_id);
  while ($solutions_data = db_fetch_object($solutions_q))
  {
    $solutions[$solutions_data->id] = $solutions_data->code_number . ' (' . $solutions_data->caption . ')';
  }
  return $solutions;
}

