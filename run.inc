<?php
// $Id$
function lab_migration_run_form($form_state)
{
  $form['#redirect'] = FALSE;

  ahah_helper_register($form, $form_state);

  /* default value for ahah fields */
  if (!isset($form_state['storage']['run']['lab']))
  {
    /* get the book id from url */
    $url_lab_id = (int)arg(1);
    if ($url_lab_id)
    {
      /* add javascript for book selected */
      $js = " $(document).ready(function() {
        $('#edit-run-lab').val(" . $url_lab_id . ");
        $('#edit-run-lab').change();
      });";
      drupal_add_js($js, 'inline', 'footer');
    } else {
      $lab_default_value = 0;
    }
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
    '#title' => t('Download Codes'),
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
    $lab_details = _lab_information($lab_default_value);

    /* solution provider */
    if ($lab_details->solution_provider_uid > 0) {
    	$user_solution_provider = user_load($lab_details->solution_provider_uid);
    	if ($user_solution_provider) {
    		$solution_provider = '<span style="color: rgb(128, 0, 0);"><strong>Solution Provider</strong></span></td><td style="width: 35%;"><br />' .
      						'<ul>' .
                  '<li><strong>Solution Provider Name:</strong> ' . $lab_details->solution_provider_name_title . ' ' . $lab_details->solution_provider_name . '</li>' .
                  '<li><strong>Department:</strong> ' . $lab_details->solution_provider_department . '</li>' .
                  '<li><strong>University:</strong> ' . $lab_details->solution_provider_university . '</li>' .
                  '</ul>';
    	} else {
    		$solution_provider = '<span style="color: rgb(128, 0, 0);"><strong>Solution Provider</strong></span></td><td style="width: 35%;"><br />' .
      						'<ul>' .
      						'<li><strong>Solution Provider: </strong> (Open) </li>' .
      						'</ul>';
    	}
    } else {
    	$solution_provider = '<span style="color: rgb(128, 0, 0);"><strong>Solution Provider</strong></span></td><td style="width: 35%;"><br />' .
      						'<ul>' .
      						'<li><strong>Solution Provider: </strong> (Open) </li>' .
      						'</ul>';
    }

    $category_data = lab_migration_get_category_name($lab_details->category);

    $form['run']['lab_details'] = array(
      '#type' => 'item',
      '#value' => '<span style="color: rgb(128, 0, 0);"><strong>About the Lab</strong></span></td><td style="width: 35%;"><br />' .
      						'<ul>' .
                  '<li><strong>Proposer Name:</strong> ' . $lab_details->name_title . ' ' . $lab_details->name . '</li>' .
                  '<li><strong>Title of the Lab:</strong> ' . $lab_details->lab_title . '</li>' .
                  '<li><strong>Department:</strong> ' . $lab_details->department . '</li>' .
                  '<li><strong>University:</strong> ' . $lab_details->university . '</li>' .
                  '<li><strong>Category:</strong> ' .  $category_data . '</li>' .
                  '</ul>' .
                  $solution_provider,
    );

    $form['run']['download_lab'] = array(
      '#type' => 'item',
      '#value' => l('Download Lab Solutions', 'lab_migration/download/lab/' . $lab_default_value),
    );
    $form['run']['download_pdf'] = array(
      '#type' => 'item',
      '#value' => l('Download PDF of Lab Solutions', 'lab_migration/generate_lab/' . $lab_default_value . '/1'),
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
        '#value' => l('Download Experiment', 'lab_migration/download/experiment/' . $experiment_default_value),
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

      $form['run']['solution_files'] = array(
        '#type'  => 'item',
        '#title' => 'List of solution files',
        '#value' => $solution_files,
      );
    }
  }
  /************ END OF $_POST **************/

  return $form;
}

function _list_of_labs()
{
  $lab_titles = array('0' => 'Please select...');
  $lab_titles_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE solution_display = 1 ORDER BY lab_title ASC");
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

function _lab_information($proposal_id)
{
	$lab_q = db_query("SELECT * FROM {lab_migration_proposal} WHERE id = %d", $proposal_id);
  $lab_data = db_fetch_object($lab_q);
  return $lab_data;
}

