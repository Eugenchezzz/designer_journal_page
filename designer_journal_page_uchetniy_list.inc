<?php

/**
 * Обрабатывает страницу для пути 'objects/edit/%/journal_nadzora/uchetniy_list'.
 */
/**
 * Обрабатывает страницу для пути 'objects/edit/%/journal_nadzora/uchetniy_list'.
 */
function designer_journal_page_uchetniy_list($object_id) {
    // сразу проверяем, кто запрашивает страницу, проектировщик и админ - ОК.
    global $user;
    if (!in_array('Проектировщик', array_values($user->roles)) && !in_array('administrator', array_values($user->roles))) {
        drupal_set_message('Доступ к журналу авторского надзора разрешен только для проектировщиков.', 'error');
        // Возвращаемся на предыдущую страницу
        drupal_goto($_SERVER['HTTP_REFERER']);
        return; // Прерываем выполнение текущей функции
    }

    // Подключаем содержимое HTML-файла sec_header1.html
    $sec_header_path = drupal_get_path('module', 'designer_journal_page') . '/templates/sec_header4.html';
    $sec_header = file_get_contents($sec_header_path);

    // Заменяем {{object_id}} на реальное значение
    $sec_header = str_replace('{{object_id}}', '' . $object_id, $sec_header);
    $output = $sec_header;

    // Загружаем объект по object_id
    $object_node = node_load($object_id);

    // Загружаем журнал от объекта
    $journal_id = $object_node->field_journal_nadzora_ref['und'][0]['target_id'];
    $journal_node = node_load($journal_id);

    // Выводим содержимое страницы


    // Формируем заголовок таблицы
    $header = array(
        t('Дата'),
        t('Запись произвел'),
        t('Ознакомлены (Организация)'),
        t('Ознакомлены (Заказчик)'),
        t('Отметка об исполнении (Производитель)'),
        t('Отметка об исполнении (Представитель)'),
        t('Нарушения'),
        t('Указания'),
        t('Действие'),
    );

    $rows = array();

    // Проходимся по элементам коллекции полей
    $index = 0;
    foreach ($journal_node->field_uchetniy_list['und'] as $item) {
        $field_collection_item = entity_load('field_collection_item', array($item['value']));
        if ($field_collection_item) {
            $item = reset($field_collection_item);

            // Извлечение даты (только дата без времени)
            $date_value = isset($item->field_list_date['und'][0]['value']) ? $item->field_list_date['und'][0]['value'] : '';
            $date_only = !empty($date_value) ? date('Y-m-d', strtotime($date_value)) : '';

            // Формируем строку для таблицы
            $rows[] = array(
                check_plain($date_only),
                isset($item->field_list_zapis_proizvel['und'][0]['value']) ? check_plain($item->field_list_zapis_proizvel['und'][0]['value']) : '',
                isset($item->field_list_oznak_org['und'][0]['value']) ? check_plain($item->field_list_oznak_org['und'][0]['value']) : '',
                isset($item->field_list_oznak_zak['und'][0]['value']) ? check_plain($item->field_list_oznak_zak['und'][0]['value']) : '',
                isset($item->field_list_otmetka_ukaz_proizv['und'][0]['value']) ? check_plain($item->field_list_otmetka_ukaz_proizv['und'][0]['value']) : '',
                isset($item->field_list_otmetka_ukaz_predst['und'][0]['value']) ? check_plain($item->field_list_otmetka_ukaz_predst['und'][0]['value']) : '',
                isset($item->field_list_narushenija['und'][0]['value']) ? check_markup($item->field_list_narushenija['und'][0]['value'], $item->field_list_narushenija['und'][0]['format']) : '',
                isset($item->field_list_ukazanija['und'][0]['value']) ? check_markup($item->field_list_ukazanija['und'][0]['value'], $item->field_list_ukazanija['und'][0]['format']) : '',
                '<button class="edit-button btn btn-default form-submit" data-index="' . $index . '"> <span class="icon glyphicon glyphicon-pencil" aria-hidden="true"></span> </button>',

            );
            $form = drupal_get_form('designer_journal_page_uchetniy_list_edit_form', $journal_id, $index);
            // Скрытая строка с формой
            $rows[] = array(
                'data' => array(
                    array(
                        'data' => drupal_render($form),
                        'colspan' => 6, // Объединяем все колонки
                    ),
                ),
                'class' => array('edit-form-row'),
                'id' => 'edit-row-' . $index,
                'style' => 'display: none;', // Скрываем форму по умолчанию
            );
        }
        $index++;
    }

    drupal_add_js("
        jQuery(document).ready(function($) {
            $('.edit-button').click(function() {
                var index = $(this).data('index');
                $('#row-' + index).hide();
                $('#edit-row-' + index).show();
            });
        });
    ", 'inline');



    // Рендерим таблицу

    $table = theme('table', array('header' => $header, 'rows' => $rows));
    $output .= '<div class="table-left">' . $table . '</div>';

    // Добавляем форму добавления новой записи
    $output .= '<style>

/* для отображения картинок */
td > p > a > img {
    max-width: 100px !important;
    max-height: 100px !important;
    width: auto;
    height: auto;
}



/* Стилизация таблицы */
.table th, .table td {
   
    padding: 8px !important; /* Отступ внутри ячейки */
    text-align: left !important;
}

/* Обрезка и перенос текста */
.table td {
    width: 100px !important; /* Статическая ширина ячейки */
    overflow: hidden !important;
    text-overflow: ellipsis !important; /* Добавляет троеточие для длинного текста */
    word-break: break-word !important; /* Переносит длинные слова */
}

/* Ширина ячеек последнего столбца */
.table td:last-child {
    width: 10px !important; /* Статическая ширина для последнего столбца */
}







.table-responsive{
width: 100% !important;
}

/* Стилизация таблицы */


.table th, .table-custom td {
    border: 1px solid #ddd !important; /* Граница ячейки таблицы */
    padding: 8px !important; /* Отступ внутри ячейки */
    text-align: left !important;
}

/* Обрезка и перенос текста */
.table td {
    width: 300px !important; /* Максимальная ширина ячейки, можете настроить */
    overflow: hidden !important;
    text-overflow: ellipsis !important; /* Добавляет троеточие для длинного текста */
   /* white-space: nowrap !important; /* Не переносит текст на новую строку */
    word-break: break-word !important; /* Переносит длинные слова */
}

/* Показ полного текста при наведении */
/*.table td:hover {
 /*   white-space: normal !important; /* Разрешает перенос текста при наведении */
 /*   background-color: #f5f5f5 !important; /* Изменение фона при наведении */
/*}



 */
.fields-inline .form-item {
    flex: 1 1 400px !important; /* Элементы могут растягиваться или сжиматься */
    min-width: 0 !important;   /* Обеспечивает сжатие элементов при нехватке места */
    max-width: 100%;           /* Предотвращает расширение элемента за пределы контейнера */
}








</style>';
    $output .= '<hr style="border: 7px solid black;  width: 100%;;">';


    // Вывод формы добавления
//    $output .= '<h1>Добавление записи</h1>';
//      $form = drupal_get_form('designer_journal_page_uchetniy_list_add_form', $journal_id);
//      $output .= drupal_render($form);
//


    return $output;
}



// ФОРМА ДОБАВЛЕНИЯ И ЕЕ ОБРАБОТЧИК
/**
 * Форма для добавления новой записи в учетный лист.
 */
function designer_journal_page_uchetniy_list_add_form($form, &$form_state, $journal_id) {
// КОД ДЛЯ СОЗДАНИЯ МАССИВОВ ДОПУСТИМЫХ ДАННЫХ ДЛЯ ВЫБОРА
// Загружаем ноду
    $journal_node = node_load($journal_id);

    $fio_variants = [];
    $index = 0;

    if ($journal_node && isset($journal_node->field_predstaviteli_nadzora['und'])) {
        foreach ($journal_node->field_predstaviteli_nadzora['und'] as $item) {
            // Загружаем элемент коллекции полей
            $field_collection_item = entity_load('field_collection_item', array($item['value']));

            if ($field_collection_item) {
                $item = reset($field_collection_item);

                // Получаем значение ФИО
                $fio_value = isset($item->field_reg_fio['und'][0]['value']) ? $item->field_reg_fio['und'][0]['value'] : 'Unknown';

                // Проверяем, есть ли уже такое ФИО в массиве
                if (!in_array($fio_value, $fio_variants)) {
                    // Добавляем ФИО в массив, если оно не встречается
                    $fio_variants[$fio_value] = $fio_value;
                    $index++;
                }
            }
        }



    } else {
        drupal_set_message(t('Node or field field_predstaviteli_nadzora not found.'));
    }



//    $form['field_container'] = array(
//        '#type' => 'container',
//        '#attributes' => array(
//            'class' => array('fields-inline', 'form-inline'),
//            'style' => 'display: flex;flex-direction: column;  align-items: center;  gap: 5px; flex-warp: warp !important; overflow-x: auto !important;max-width:1500px !important;', // Flex-wrap для переноса на новую строку при нехватке места
//        ),
//
//    );

    $form['field_list_zapis_proizvel'] = array(
        '#type' => 'select',
        '#title' => t('Запись произвел'),
        '#options' => $fio_variants,
        '#required' => TRUE,
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['field_list_date'] = array(
        '#type' => 'date_popup',
        '#title' => t('Дата'),
        '#default_value' => !empty($field_collection_item->field_list_date['und'][0]['value'])
            ? array(
                'year' => date('Y', strtotime($field_collection_item->field_list_date['und'][0]['value'])),
                'month' => date('n', strtotime($field_collection_item->field_list_date['und'][0]['value'])),
                'day' => date('j', strtotime($field_collection_item->field_list_date['und'][0]['value']))
            )
            : array('year' => '', 'month' => '', 'day' => ''),
        '#date_date_element' => 'date',
        '#date_month_element' => 'month',
        '#date_year_element' => 'year',
        '#date_day_element' => 'day',
        '#date_format' => 'Y-m-d', // Обратите внимание на формат даты
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['field_list_narushenija'] = array(
        '#type' => 'text_format',
        '#title' => t('Нарушения'),
        '#format'=> 'full_html',
    );



    $form['field_list_ukazanija'] = array(
        '#type' => 'text_format',
        '#title' => t('Указания'),
        '#format'=> 'full_html',
    );

    $form['field_list_oznak_org'] = array(
      '#type' => 'textfield',
      '#title' => t('Ознакомлены (Организация)'),
    );

    $form['field_list_oznak_zak'] = array(
        '#type' => 'textfield',
        '#title' => t('Ознакомлены (Заказчик)'),
    );

    $form['field_list_otmetka_ukaz_proizv'] = array(
        '#type' => 'textfield',
        '#title' => t('Отметка о выполнении указаний (Производитель)'),
    );

    $form['field_list_otmetka_ukaz_predst'] = array(
        '#type' => 'textfield',
        '#title' => t('Отметка о выполнении указаний (Представитель)'),
    );

            // Сохраняем объект журнала в form_state для использования в обработчике
    $form_state['journal_node'] = node_load($journal_id);



    // Кнопка сабмита
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Добавить запись'),
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    return $form;
}



/**
 * Обработчик сабмита для формы добавления новой записи в учетный лист.
 */
function designer_journal_page_uchetniy_list_add_form_submit($form, &$form_state) {
    // Получаем ноду из состояния формы
    $journal_node = $form_state['journal_node'];

    // Проверяем, что нода существует
    if ($journal_node && isset($journal_node->nid)) {
        // Получаем данные из формы
        $date = isset($form_state['values']['field_list_date']) ? $form_state['values']['field_list_date'] : array();
        $record_by = isset($form_state['values']['field_list_zapis_proizvel']) ? $form_state['values']['field_list_zapis_proizvel'] : '';



        // Создаем новый элемент коллекции полей для учетного листа
        $field_collection_item = entity_create('field_collection_item', array('field_name' => 'field_uchetniy_list'));
        $field_collection_item->setHostEntity('node', $journal_node);

        // Устанавливаем значения полей
        $field_collection_item->field_list_date['und'][0]['value'] = $date;
        $field_collection_item->field_list_zapis_proizvel['und'][0]['value'] = $record_by;
        // играемся с полем ckeditor

        //загоняем данные всратого поля с html кодом (текст и картинки)(нарушения)
        $instructions = isset($form_state['values']['field_list_narushenija']) ? $form_state['values']['field_list_narushenija'] : '';
        $text = $instructions['value']; // Исходный текст

        // Ищем все теги <img> и оборачиваем их в ссылки на полные изображения
        $pattern = '/<img(.*?)src="(.*?)"(.*?)>/i';
        $replacement = '<a href="$2" class="colorbox" rel="gallery"><img$1src="$2"$3></a>';
        $text_with_colorbox = preg_replace($pattern, $replacement, $text);

        $field_collection_item->field_list_narushenija['und'][0]['value'] = $text_with_colorbox;
        $field_collection_item->field_list_narushenija['und'][0]['format'] = $instructions['format'];

        //загоняем для другого поля - указания

        $instructions = isset($form_state['values']['field_list_ukazanija']) ? $form_state['values']['field_list_ukazanija'] : '';
        $text = $instructions['value']; // Исходный текст

        // Ищем все теги <img> и оборачиваем их в ссылки на полные изображения
        $pattern = '/<img(.*?)src="(.*?)"(.*?)>/i';
        $replacement = '<a href="$2" class="colorbox" rel="gallery"><img$1src="$2"$3></a>';
        $text_with_colorbox = preg_replace($pattern, $replacement, $text);

        $field_collection_item->field_list_ukazanija['und'][0]['value'] = $text_with_colorbox;
        $field_collection_item->field_list_ukazanija['und'][0]['format'] = $instructions['format'];

        //загоняем базовые текстовые поля
        // Поле field_list_oznak_org
        $oznak_org = isset($form_state['values']['field_list_oznak_org']) ? $form_state['values']['field_list_oznak_org'] : '';
        $field_collection_item->field_list_oznak_org['und'][0]['value'] = $oznak_org;

        // Поле field_list_oznak_zak
        $oznak_zak = isset($form_state['values']['field_list_oznak_zak']) ? $form_state['values']['field_list_oznak_zak'] : '';
        $field_collection_item->field_list_oznak_zak['und'][0]['value'] = $oznak_zak;

        // Поле field_list_otmetka_ukaz_proizv
        $otmetka_proizv = isset($form_state['values']['field_list_otmetka_ukaz_proizv']) ? $form_state['values']['field_list_otmetka_ukaz_proizv'] : '';
        $field_collection_item->field_list_otmetka_ukaz_proizv['und'][0]['value'] = $otmetka_proizv;

        // Поле field_list_otmetka_ukaz_predst
        $otmetka_predst = isset($form_state['values']['field_list_otmetka_ukaz_predst']) ? $form_state['values']['field_list_otmetka_ukaz_predst'] : '';
        $field_collection_item->field_list_otmetka_ukaz_predst['und'][0]['value'] = $otmetka_predst;



        // Сохраняем новый элемент коллекции полей
        $field_collection_item->save();
        drupal_set_message($date);
        drupal_set_message();
        drupal_set_message();
        drupal_set_message();
        // Перенаправляем пользователя на страницу журнала после добавления записи

        $form_state['redirect'] = $_SERVER['REQUEST_URI'];



    } else {
        form_set_error('', t('Нода журнала не найдена.'));
    }
}







// ЗДЕСЬ КОД ДЛЯ ФОРМЫ РЕДАКТИРОВАНИЯ
function designer_journal_page_uchetniy_list_edit_form($form, &$form_state, $journal_id, $row_index) {
    // Загружаем ноду
    $journal_node = node_load($journal_id);
    $item = $journal_node->field_uchetniy_list['und'][$row_index];
    $field_collection_item = entity_load('field_collection_item', array($item['value']));


    $item = reset($field_collection_item);


    $row = $item;



    // Получаем варианты ФИО
    $fio_variants = [];
    $index = 0;
    if ($journal_node && isset($journal_node->field_predstaviteli_nadzora['und'])) {
        foreach ($journal_node->field_predstaviteli_nadzora['und'] as $item) {
            $field_collection_item = entity_load('field_collection_item', array($item['value']));
            if ($field_collection_item) {
                $item = reset($field_collection_item);
                $fio_value = isset($item->field_reg_fio['und'][0]['value']) ? $item->field_reg_fio['und'][0]['value'] : 'Unknown';
                if (!in_array($fio_value, $fio_variants)) {
                    $fio_variants[$fio_value] = $fio_value;
                    $index++;
                }
            }
        }
    }








    // Поле 'field_list_zapis_proizvel'
//    $form['field_list_zapis_proizvel'] = array(
//        '#type' => 'select',
//        '#title' => t('Запись произвел'),
//        '#options' => $fio_variants,
//        '#required' => TRUE,
//        '#default_value' => '$default_value',
//        '#attributes' => array(
//            'class' => array('form-field-inline'),
//            'style' => 'margin-right: 10px;',
//        ),
//    );


    // Поле 'field_list_date'
    $form['field_list_date'] = array(
        '#type' => 'date_popup',
        '#title' => t('Дата'),
        '#default_value' => $row && !empty($row->field_list_date['und'][0]['value'])
            ? array(
                'year' => date('Y', strtotime($row->field_list_date['und'][0]['value'])),
                'month' => date('n', strtotime($row->field_list_date['und'][0]['value'])),
                'day' => date('j', strtotime($row->field_list_date['und'][0]['value']))
            )
            : array('year' => '', 'month' => '', 'day' => ''),
        '#date_date_element' => 'date',
        '#date_month_element' => 'month',
        '#date_year_element' => 'year',
        '#date_day_element' => 'day',
        '#date_format' => 'Y-m-d',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    dsm($row->field_list_narushenija['und'][0]['value']);
    // Поле 'field_list_narushenija' с подгрузкой формата и значения
    $form['field_list_narushenija'] = array(
        '#type' => 'text_format',
        '#title' => t('Нарушения'),
      '#default_value' => isset($row->field_list_narushenija['und'][0]['value']) ? $row->field_list_narushenija['und'][0]['value'] : '',

    );

    // Поле 'field_list_ukazanija' с подгрузкой формата и значения
    $form['field_list_ukazanija'] = array(
        '#type' => 'text_format',
        '#title' => t('Указания'),
       '#default_value' => isset($row->field_list_ukazanija['und'][0]['value']) ? $row->field_list_ukazanija['und'][0]['value'] : '',

    );

    // Поле 'field_list_oznak_org'
    $form['field_list_oznak_org'] = array(
        '#type' => 'textfield',
        '#title' => t('Ознакомлены (Организация)'),
        '#default_value' => isset($row->field_list_oznak_org['und'][0]['value']) ? $row->field_list_oznak_org['und'][0]['value'] : '',
    );

    // Поле 'field_list_oznak_zak'
    $form['field_list_oznak_zak'] = array(
        '#type' => 'textfield',
        '#title' => t('Ознакомлены (Заказчик)'),
     '#default_value' => isset($row->field_list_oznak_zak['und'][0]['value'])? $row->field_list_oznak_zak['und'][0]['value'] : '',
    );

    // Поле 'field_list_otmetka_ukaz_proizv'
    $form['field_list_otmetka_ukaz_proizv'] = array(
        '#type' => 'textfield',
        '#title' => t('Отметка о выполнении указаний (Производитель)'),
        '#default_value' => isset($row->field_list_otmetka_ukaz_proizv['und'][0]['value']) ? $row->field_list_otmetka_ukaz_proizv['und'][0]['value'] : '',
    );




    // Поле 'field_list_otmetka_ukaz_predst'
    $form['field_list_otmetka_ukaz_predst'] = array(
        '#type' => 'textfield',
        '#title' => t('Отметка о выполнении указаний (Представитель)'),
        '#default_value' => isset($row->field_list_otmetka_ukaz_predst['und'][0]['value']) ? $row->field_list_otmetka_ukaz_predst['und'][0]['value'] : '',

    );

    // Кнопка сабмита
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Добавить запись'),
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    return $form;
}
