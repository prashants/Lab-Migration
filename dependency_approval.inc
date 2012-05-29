<?php

/******************************************************************************/
/********************************* BULK APPROVAL ******************************/
/******************************************************************************/

function lab_migration_dependency_approval_form($form_state)
{
  $form['#redirect'] = FALSE;

  ahah_helper_register($form, $form_state);

  /* default value for ahah fields */
  if (!isset($form_state['storage']['run']['dependency']))
  {
    $dependency_default_value = 0;
  } else {
    $dependency_default_value =  $form_state['storage']['run']['dependency'];
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

  $form['run']['dependency'] = array(
      '#type' => 'select',
      '#title' => t('Dependency'),
      '#options' => _list_of_dependencies(),
      '#default_value' => $dependency_default_value,
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

  $solution_list = array();  
  /************ START OF $_POST **************/
  if ($_POST)
  {
    if ($dependency_default_value > 0)
    {
    	$solution_id_q = db_query("SELECT * FROM {lab_migration_solution_dependency} WHERE dependency_id = %d", $dependency_default_value);
    	while ($solution_id_data = db_fetch_object($solution_id_q)) {
    		$solution_q = db_query("SELECT * FROM {lab_migration_solution} WHERE id = %d", $solution_id_data->solution_id);
    		$solution_data = db_fetch_object($solution_q);
    		$experiment_q = db_query("SELECT * FROM {lab_migration_experiment} WHERE id = %d", $solution_data->experiment_id);
    		$experiment_data = db_fetch_object($experiment_q);
    		$lab_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $experiment_data->proposal_id);
    		$lab_data = db_fetch_object($lab_q);
    		$solution_list[] = array($solution_data->code_number, $experiment_data->number . ' . ' . $experiment_data->title, $lab_data->lab_title);
    	}
			$solution_list_header = array('Code', 'Experiment', 'Lab');
			$solution = theme_table($solution_list_header, $solution_list); 

			if ($solution_list) {
				$form['run']['solution_dependency'] = array(
					'#type'  => 'item',
					'#value' => $solution,
				);
				$form['run']['solution_dependency_message'] = array(
					'#type'  => 'item',
					'#value' => 'Please unlink the dependency from the above solutions before deleting it',
				);
			}
    }
  }

  /* hidden form elements */
  $form['run']['dependency_hidden'] = array(
    '#type'  => 'hidden',
    '#value' =>  $form_state['values']['run']['dependency'],
  );

  if (!$solution_list && $dependency_default_value > 0)
  {
    $form['run']['delete_dependency'] = array(
      '#type'  => 'checkbox',
      '#title' => t('Delete Dependency'),
    );
    $form['run']['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Submit')
    );
  }

  return $form;
}


function lab_migration_dependency_approval_form_submit($form, &$form_state)
{
	global $user;
  $root_path = lab_migration_path();

	if ($form_state['clicked_button']['#value'] == 'Submit')
	{
	  if (user_access('bulk manage code'))
    {
      if ($form_state['values']['run']['delete_dependency'] == "1")
      {
				$solution_q = db_query("SELECT * FROM {lab_migration_solution_dependency} WHERE dependency_id = %d", $form_state['values']['run']['dependency']);
				if ($solution_data = db_fetch_object($solution_q)) {
					drupal_set_message('Cannot delete dependency since it is linked with some solutions' , 'error');
				} else {
					if (lab_migration_delete_dependency($form_state['values']['run']['dependency']))
					{
						drupal_set_message('Dependency deleted' , 'status');

						/* email */
						$email_subject = t('Dependency deleted');
						$email_body = t('Dependency deleted : .') . $form_state['values']['run']['dependency'];
						$email_to = variable_get('lab_migration_emails', '') . ', ' . $user->mail;
						$param['standard']['subject'] = $email_subject;
						$param['standard']['body'] = $email_body;
						if (!drupal_mail('lab_migration', 'standard', $email_to, language_default(), $param, variable_get('lab_migration_from_email', NULL), TRUE))
							drupal_set_message('Error sending email message.', 'error');
					}
				}
      }
    }
  }
}

function _list_of_dependencies()
{
  $dependencies = array('0' => 'Please select...');
  $dependency_q = db_query("SELECT * FROM {lab_migration_dependency_files} ORDER BY filename ASC");
  while ($dependency_data = db_fetch_object($dependency_q))
  {
    $dependencies[$dependency_data->id] = $dependency_data->filename . ' (' . $dependency_data->filepath . ')';
  }
  return $dependencies;
}

