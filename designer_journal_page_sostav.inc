<?php

/**
 * Обрабатывает страницу для пути 'objects/edit/%/journal_nadzora/sostav'.
 */
function designer_journal_page_sostav($object_id) {
  // Проверяем, что пользователь авторизован и имеет необходимые права (если нужно)
  if (!user_access('access content')) {
    drupal_access_denied();
    return;
  }

  // Подключаем содержимое HTML-файла sec_header1.html
  $sec_header_path = drupal_get_path('module', 'designer_journal_page') . '/templates/sec_header2.html';
  $sec_header = file_get_contents($sec_header_path);

  // Заменяем {{object_id}} на реальное значение
  $sec_header = str_replace('{{object_id}}', '' . $object_id, $sec_header);
  $output = $sec_header;
  // Загружаем объект по object_id
  $object_node = node_load($object_id);
  //загружаем журнал от объекта
  $journal_id = $object_node->field_journal_nadzora_ref['und'][0]['target_id'];
  $journal_node = node_load($journal_id);

  if (!$journal_node || empty($journal_node->field_sostav_rbochei)) {
    return t('No data available.');
  }

  // Start building the table
  $header = array(
    t('ФИО'),
    t('Должность, место работы'),
    t('Работа, по которой осуществяется авторский надзор'),
    t('Дата и номер приказа о назначении'),
  );

  $rows = array();

  // Loop through the field collection items
  foreach ($journal_node->field_sostav_rbochei['und'] as $item) {
    $field_collection_item = entity_load('field_collection_item', array($item['value']));
    if ($field_collection_item) {
      $item = reset($field_collection_item);

      $rows[] = array(
        isset($item->field_fio['und'][0]['value']) ? check_plain($item->field_fio['und'][0]['value']) : '',
        isset($item->field_dolzhnost['und'][0]['value']) ? check_plain($item->field_dolzhnost['und'][0]['value']) : '',
        isset($item->field_jobname['und'][0]['value']) ? check_plain($item->field_jobname['und'][0]['value']) : '',
        isset($item->field_datenum['und'][0]['value']) ? check_plain($item->field_datenum['und'][0]['value']) : '',
      );
    }
  }










  // Выводим содержимое страницы
  $output .= '<h1>' . t('Состав рабочей группы') . '</h1>';
  $output .= theme('table', array('header' => $header, 'rows' => $rows)); // рендер таблицы

  $output .= '<h1>' . t('Добавление в состав') . '</h1>';

  $form = drupal_get_form('designer_journal_sostav_rbochei_form', $journal_node);
  $output .= drupal_render($form);


  return $output;
}



//ФОРМА ДОБАВЛЕНИЯ


function designer_journal_sostav_rbochei_form($form, &$form_state, $journal_node) {
  // Поля формы
  $form['fields_container'] = array(
    '#type' => 'container',
    '#attributes' => array('class' => array('fields-inline')),
  );

  $form['fields_container']['field_fio'] = array(
    '#type' => 'textfield',
    '#title' => t('ФИО'),
    '#required' => TRUE,
    '#default_value' => '',
    '#attributes' => array('class' => array('form-field-inline')),
  );

  $form['fields_container']['field_dolzhnost'] = array(
    '#type' => 'textfield',
    '#title' => t('Должность, место работы'),
    '#required' => TRUE,
    '#default_value' => '',
    '#attributes' => array('class' => array('form-field-inline')),
  );

  $form['fields_container']['field_jobname'] = array(
    '#type' => 'textfield',
    '#title' => t('Работа, по которой осуществляется авторский надзор'),
    '#required' => TRUE,
    '#default_value' => '',
    '#attributes' => array('class' => array('form-field-inline')),
  );

  $form['fields_container']['field_datenum'] = array(
    '#type' => 'textfield',
    '#title' => t('Дата и номер приказа о назначении'),
    '#required' => TRUE,
    '#default_value' => '',
    '#attributes' => array('class' => array('form-field-inline')),
  );

  // Кнопка для сохранения формы
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Добавить'),
    '#attributes' => array('class' => array('form-submit-inline')),
  );

  // Передаем узел журнала в форму
  $form_state['journal_node'] = $journal_node;

  return $form;
}


function designer_journal_sostav_rbochei_form_submit($form, &$form_state) {
  // Получаем ноду журнала
  $journal_node = $form_state['journal_node'];

  // Создаем новый элемент в коллекции
  $field_collection_item = entity_create('field_collection_item', array('field_name' => 'field_sostav_rbochei'));

  // Устанавливаем значения для полей
  $field_collection_item->setHostEntity('node', $journal_node);
  $field_collection_item->field_fio['und'][0]['value'] = $form_state['values']['field_fio'];
  $field_collection_item->field_dolzhnost['und'][0]['value'] = $form_state['values']['field_dolzhnost'];
  $field_collection_item->field_jobname['und'][0]['value'] = $form_state['values']['field_jobname'];
  $field_collection_item->field_datenum['und'][0]['value'] = $form_state['values']['field_datenum'];

  // Сохраняем элемент коллекции
  $field_collection_item->save();
  drupal_set_message('Успешно добавлено!');
  // Перезагружаем страницу после сохранения
  $form_state['redirect'] = $_GET['q'];
}




