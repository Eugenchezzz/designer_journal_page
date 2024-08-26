<?php

/**
 * Обрабатывает страницу для пути 'objects/edit/%/journal_nadzora/sostav'.
 */
function designer_journal_page_nadzor_predstaviteli($object_id) {
  // Проверяем, что пользователь авторизован и имеет необходимые права (если нужно)
  if (!user_access('access content')) {
    drupal_access_denied();
    return;
  }

  // Подключаем содержимое HTML-файла sec_header1.html
  $sec_header_path = drupal_get_path('module', 'designer_journal_page') . '/templates/sec_header3.html';
  $sec_header = file_get_contents($sec_header_path);

  // Заменяем {{object_id}} на реальное значение
  $sec_header = str_replace('{{object_id}}', '' . $object_id, $sec_header);
  $output = $sec_header;
  // Загружаем объект по object_id
  $object_node = node_load($object_id);
  //загружаем журнал от объекта
  $journal_id = $object_node->field_journal_nadzora_ref['und'][0]['target_id'];
  $journal_node = node_load($journal_id);

  // Выводим содержимое страницы
  $output .= '<h1>' . t('Регистрация представителей авторского надзора') . '</h1>';



  //ВЫВОД ДАННЫХ В ТАБЛИЦЕ
  // Start building the table
  $header = array(
    t('Фамилия, инициалы'),
    t('Место работы'),
    t('Занимаемая должность'),
    t('Номер телефона'),
    t('Дата приезда'),
    t('Дата отъезда'),
  );

  $rows = array();

  // цикл записи в табличку для отображения
  foreach ($journal_node->field_predstaviteli_nadzora['und'] as $item) {
    $field_collection_item = entity_load('field_collection_item', array($item['value']));
    if ($field_collection_item) {
      $item = reset($field_collection_item);

      $rows[] = array(
        isset($item->field_reg_fio['und'][0]['value']) ? check_plain($item->field_reg_fio['und'][0]['value']) : '',
        isset($item->field_reg_place['und'][0]['value']) ? check_plain($item->field_reg_place['und'][0]['value']) : '',
        isset($item->field_reg_dolzhnost['und'][0]['value']) ? check_plain($item->field_reg_dolzhnost['und'][0]['value']) : '',
        isset($item->field_reg_phone['und'][0]['value']) ? check_plain($item->field_reg_phone['und'][0]['value']) : '',
        isset($item->field_reg_start_date['und'][0]['value']) ? check_plain($item->field_reg_start_date['und'][0]['value']) : '',
        isset($item->field_reg_end_date['und'][0]['value']) ? check_plain($item->field_reg_end_date['und'][0]['value']) : '',
      );
    }
  }

  // Render the table
  $output .= theme('table', array('header' => $header, 'rows' => $rows));


  $form = drupal_get_form('designer_journal_predstaviteli_nadzora_form', $journal_node);
  $output .= drupal_render($form);






  return $output;
}



// ФОРМА ДОБАВЛЕНИЯ ПРЕДСТАВИТЕЛЯ НАДЗОРА

/**
 * Функция для создания формы добавления данных в коллекцию полей field_predstaviteli_nadzora.
 */
function designer_journal_predstaviteli_nadzora_form($form, &$form_state, $journal_node) {
  // Создаем форму с пустыми значениями по умолчанию
  $form['field_reg_fio'] = array(
    '#type' => 'textfield',
    '#title' => t('ФИО'),
    '#default_value' => '',
  );

  $form['field_reg_place'] = array(
    '#type' => 'textfield',
    '#title' => t('Место'),
    '#default_value' => '',
  );

  $form['field_reg_dolzhnost'] = array(
    '#type' => 'textfield',
    '#title' => t('Должность'),
    '#default_value' => '',
  );

  $form['field_reg_phone'] = array(
    '#type' => 'textfield',
    '#title' => t('Телефон'),
    '#default_value' => '',
  );

  $form['field_reg_start_date'] = array(
    '#type' => 'date',
    '#title' => t('Дата начала'),
    '#default_value' => '',
  );

  $form['field_reg_end_date'] = array(
    '#type' => 'date',
    '#title' => t('Дата окончания'),
    '#default_value' => '',
  );


  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Сохранить'),
  );

  // Передаем узел журнала в форму
  $form_state['journal_node'] = $journal_node;

  return $form;
}


function designer_journal_predstaviteli_nadzora_form_submit($form, &$form_state) {
  // Получаем ноду журнала
  $journal_node = $form_state['journal_node'];

  // Проверяем, что нода журнала существует
  if (!$journal_node) {
    drupal_set_message(t('Не удалось найти журнал.'), 'error');
    return;
  }

  // Создаем новый элемент в коллекции
  $field_collection_item = entity_create('field_collection_item', array(
    'field_name' => 'field_predstaviteli_nadzora',
  ));




  // Устанавливаем значения для полей
  $field_collection_item->setHostEntity('node', $journal_node);
  $field_collection_item->field_reg_fio['und'][0]['value'] = $form_state['values']['field_reg_fio'];
  $field_collection_item->field_reg_place['und'][0]['value'] = $form_state['values']['field_reg_place'];
  $field_collection_item->field_reg_dolzhnost['und'][0]['value'] = $form_state['values']['field_reg_dolzhnost'];
  $field_collection_item->field_reg_phone['und'][0]['value'] = $form_state['values']['field_reg_phone'];
//  $field_collection_item->field_reg_start_date['und'][0]['value'] = $form_state['values']['field_reg_start_date'];
//  $field_collection_item->field_reg_end_date['und'][0]['value'] = $form_state['values']['field_reg_end_date'];





  // Сохраняем элемент коллекции
  $field_collection_item->save();



  // Отправляем сообщение об успешном добавлении
  drupal_set_message(t('Представитель успешно добавлен!'));

  // Перезагружаем текущую страницу
  $form_state['redirect'] = $_GET['q'];
}




