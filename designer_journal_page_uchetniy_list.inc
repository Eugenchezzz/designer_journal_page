<?php

/**
 * Обрабатывает страницу для пути 'objects/edit/%/journal_nadzora/uchetniy_list'.
 */
function designer_journal_page_uchetniy_list($object_id) {
  // Проверяем, что пользователь авторизован и имеет необходимые права (если нужно)
  if (!user_access('access content')) {
    drupal_access_denied();
    return;
  }

  // Подключаем содержимое HTML-файла sec_header1.html
  $sec_header_path = drupal_get_path('module', 'designer_journal_page') . '/templates/sec_header4.html';
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
  $output .= '<h1>' . t('Учетный лист') . '</h1>';


  //ВЫВОД ЛИСТА

  // Формируем заголовок таблицы
  $header = array(
    t('Дата'),
    t('Нарушения'),
    t('Указания'),
    t('Запись произвел'),
    t('Ознакомлены (Организация)'),
    t('Ознакомлены (Заказчик)'),
    t('Отметка об исполнении (Производитель)'),
    t('Отметка об исполнении (Представитель)'),
  );

  $rows = array();

  // Проходимся по элементам коллекции полей
  foreach ($journal_node->field_uchetniy_list['und'] as $item) {
    $field_collection_item = entity_load('field_collection_item', array($item['value']));
    if ($field_collection_item) {
      $item = reset($field_collection_item);

      // Формируем строку для таблицы
      $rows[] = array(
        isset($item->field_list_date['und'][0]['value']) ? check_plain($item->field_list_date['und'][0]['value']) : '',
        isset($item->field_list_narushenija['und'][0]['value']) ? check_plain($item->field_list_narushenija['und'][0]['value']) : '',
        isset($item->field_list_ukazanija['und'][0]['value']) ? check_plain($item->field_list_ukazanija['und'][0]['value']) : '',
        isset($item->field_list_zapis_proizvel['und'][0]['value']) ? check_plain($item->field_list_zapis_proizvel['und'][0]['value']) : '',
        isset($item->field_list_oznak_org['und'][0]['value']) ? check_plain($item->field_list_oznak_org['und'][0]['value']) : '',
        isset($item->field_list_oznak_zak['und'][0]['value']) ? check_plain($item->field_list_oznak_zak['und'][0]['value']) : '',
        isset($item->field_list_otmetka_ukaz_proizv['und'][0]['value']) ? check_plain($item->field_list_otmetka_ukaz_proizv['und'][0]['value']) : '',
        isset($item->field_list_otmetka_ukaz_predst['und'][0]['value']) ? check_plain($item->field_list_otmetka_ukaz_predst['und'][0]['value']) : '',
      );
    }
  }

  // Рендерим таблицу
  $output .= theme('table', array('header' => $header, 'rows' => $rows));


  return $output;
}
