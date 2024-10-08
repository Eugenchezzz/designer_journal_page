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
    $row_index = 0;
    if (isset($journal_node->field_uchetniy_list['und']) && is_array($journal_node->field_uchetniy_list['und'])) {
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
                    // Объединяем обе кнопки в одну ячейку
                    '<button class="open-history-button btn btn-default form-submit" data-index="' . $row_index . '" data-objectID="' . $object_id . '"> <span class="icon glyphicon glyphicon-time" title="История изменений" aria-hidden="true"></span> </button> 
                 <button class="edit-button btn btn-default form-submit" data-index="' . $row_index . '"> <span class="icon glyphicon glyphicon-pencil" aria-hidden="true"></span> </button>',
                );

                $form = drupal_get_form('designer_journal_page_uchetniy_list_edit_form', $journal_id, $row_index);

                // Скрытая строка с формой (форма изначально скрыта через display: none)
                $rows[] = array(
                    'data' => array(
                        array(
                            // Оборачиваем форму в div, который будет открываться в Colorbox
                            'data' => '<div id="edit-row-' . $row_index . '" style="display: none;">' . drupal_render($form) . '</div>',
                            'colspan' => 9, // Объединяем все колонки
                        ),
                    ),
                    'class' => array('edit-form-row'),
                    'id' => 'edit-row-container-' . $row_index, // Уникальный идентификатор для контейнера
                );


            }
            $row_index++;
        }
    }


    // Обработчик нажатия на кнопку вне цикла

    drupal_add_js("
    jQuery(document).ready(function($) {
        // Создаем затемняющий элемент
        $('body').append('<div class=\"overlay\" style=\"display: none; position: fixed; top: 0; left: 0; right: 0; bottom: 0; background-color: rgba(0, 0, 0, 0.5); z-index: 9998;\"></div>');

        $('.edit-button').click(function() {
            var index = $(this).data('index');
            $('#row-' + index).hide();
            $('#edit-row-' + index).show();
            $('.overlay').show(); // Показываем затемняющий элемент
        });

        // Добавляем обработчик для скрытия элемента и затемнения
        $('.overlay').click(function() {
            var index = $('.edit-button:visible').data('index'); // Получаем индекс видимой кнопки
            $('#edit-row-' + index).hide();
            $(this).hide(); // Скрываем затемняющий элемент
            $('#row-' + index).show(); // Показываем основной элемент
        });
    });
", 'inline');






// Рабочий обработчик открытия формы редактирования (без колорбокса)
//    drupal_add_js("
//        jQuery(document).ready(function($) {
//            $('.edit-button').click(function() {
//                var index = $(this).data('index');
//                $('#row-' + index).hide();
//                $('#edit-row-' + index).show();
//            });
//        });
//    ", 'inline');

    //ТАК КАК СДЕЛАТЬ КОЛОРБОКС ДЛЯ ФОРМЫ РЕДАКТИРОВАНИЯ НЕ ПОЛУЧИЛОСЬ, СТИЛЯМИ СДЕЛАЕМ ЭТОТ ДИВ ПОХОЖИМ НА НЕГО
    $output .= '<style>


form[id^="designer-journal-page-uchetniy-list-edit-form"] {
  padding: 3rem;
}


div[id^="edit-row-"] {
border: none;
}
    div[id^="edit-row-"] {
        
        position: absolute;
        z-index: 10000;
        width: 110%;
        background-color: rgb(255,255,255);
        border-radius: 10px;
        top: 40%; /* Центрируем по вертикали */
        left: 50%; /* Центрируем по горизонтали */
        transform: translate(-50%, -50%); /* Сдвигаем на 50% и поворачиваем на 45 градусов */
    }
</style>';






    // Рендерим таблицу

    $table = theme('table', array('header' => $header, 'rows' => $rows));
    $output .= '<div class="table-left">' . $table . '</div>';


    $output .= '<style>

/* ПЕРЕСТАНОВКА КНОПОЧКИ В КОЛОРБОКСЕ */
#cboxClose{
    position: absolute !important;
    top: 1px !important;
}

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


.ckeditor_links, .panel-body {
    display: none !important;
}


/* ДЕЛАЕМ СТИЛИ КАК У ВЛАДА */


/* Убираем границы в хедере таблицы, оставляя нижнее подчеркивание */
.table.table-hover.table-striped.sticky-enabled.tableheader-processed.sticky-table thead th {
    border-top: none !important;
    border-left: none !important;
    border-right: none !important;

}

/* Чередуем цвета строк таблицы */

/* Устанавливаем цвета строк: 2 через 2 */
.table.table-hover.table-striped.sticky-enabled.tableheader-processed.sticky-table tbody tr:nth-child(4n-3),
.table.table-hover.table-striped.sticky-enabled.tableheader-processed.sticky-table tbody tr:nth-child(4n-2) {
    background-color: #f2f7fd; /* Белый фон для первых двух строк в каждой группе */
}

.table.table-hover.table-striped.sticky-enabled.tableheader-processed.sticky-table tbody tr:nth-child(4n-1),
.table.table-hover.table-striped.sticky-enabled.tableheader-processed.sticky-table tbody tr:nth-child(4n) {
    background-color: #e8ecf3; /* Серый фон для следующих двух строк в каждой группе */
}








</style>';


    // Добавляем форму добавления новой записи

    $output .= '<h1>Добавление записи</h1>';
    // Вывод формы добавления
      $form = drupal_get_form('designer_journal_page_uchetniy_list_add_form', $journal_id);
      $output .= drupal_render($form);

// ОБРАБОТКА КНОПКИ ЖУРНАЛА

    $history_items = array();

    if (isset($journal_node->field_history_uchetniy_list['und']) && is_array($journal_node->field_history_uchetniy_list['und'])) {
        foreach ($journal_node->field_history_uchetniy_list['und'] as $item) {
            $field_collection_item = entity_load('field_collection_item', array($item['value']));
            if ($field_collection_item) {
                $history_item = reset($field_collection_item);

                $history_items[] = [
                    'rowid' => isset($history_item->field_history_list_rowid['und'][0]['value']) ? $history_item->field_history_list_rowid['und'][0]['value'] : '',
                    //'zapis_proizvel' => isset($history_item->field_history_list_zapis_proizve['und'][0]['value']) ? $history_item->field_history_list_zapis_proizve['und'][0]['value'] : '',
                    'date' => isset($history_item->field_history_list_date['und'][0]['value']) ? $history_item->field_history_list_date['und'][0]['value'] : '',
                    'narushenija' => isset($history_item->field_history_list_narushenija['und'][0]['value']) ? $history_item->field_history_list_narushenija['und'][0]['value'] : '',
                    'ukazanija' => isset($history_item->field_history_list_ukazanija['und'][0]['value']) ? $history_item->field_history_list_ukazanija['und'][0]['value'] : '',
                    'oznak_org' => isset($history_item->field_history_list_oznak_org['und'][0]['value']) ? $history_item->field_history_list_oznak_org['und'][0]['value'] : '',
                    'oznak_zak' => isset($history_item->field_history_list_oznak_zak['und'][0]['value']) ? $history_item->field_history_list_oznak_zak['und'][0]['value'] : '',
                    'otmetka_ukaz_proizv' => isset($history_item->field_history_list_ukaz_proizv['und'][0]['value']) ? $history_item->field_history_list_ukaz_proizv['und'][0]['value'] : '',
                    'otmetka_ukaz_predst' => isset($history_item->field_history_list_ukaz_predst['und'][0]['value']) ? $history_item->field_history_list_ukaz_predst['und'][0]['value'] : '',
                ];
            }
        }
    }

    dsm($history_item);
    $json_history_data = json_encode($history_items);


    // Обработка кнопки изменений
    drupal_add_js("
    
    jQuery(document).ready(function($) {
        // Данные истории в JSON формате
        var historyData = " . $json_history_data . ";

        // Обработчик клика на кнопку
        $('.open-history-button').click(function() {
            var index = $(this).data('index').toString(); // Преобразование индекса в строку

            var tableContent = '<table class=\"table\"><thead><tr>' +
                '<th>Дата</th>' +
                '<th>Нарушения</th>' +
                '<th>Указания</th>' +
                '<th>Ознакомлен орг.</th>' +
                '<th>Ознакомлен заказчик</th>' +
                '<th>Отметка указания произв.</th>' +
                '<th>Отметка указания предст.</th>' +
            '</tr></thead><tbody>';

            var rows = [];

            // Фильтрация данных по значению индекса и добавление строк в массив
            $.each(historyData, function(i, item) {
                var rowidStr = item.rowid.toString(); // Преобразование rowid в строку
                var match = (rowidStr === index) ? 'Match' : 'No match'; // Результат сравнения

                // Добавление строки в массив только если есть совпадение
                if (match === 'Match') {
                    rows.push(
                        '<tr>' +
                        '<td>' + item.date + '</td>' +
                        '<td>' + item.narushenija + '</td>' +
                        '<td>' + item.ukazanija + '</td>' +
                        '<td>' + item.oznak_org + '</td>' +
                        '<td>' + item.oznak_zak + '</td>' +
                        '<td>' + item.otmetka_ukaz_proizv + '</td>' +
                        '<td>' + item.otmetka_ukaz_predst + '</td>' +
                    '</tr>'
                    );
                }
            });

            // Добавление строк в таблицу в обратном порядке
            rows.reverse().forEach(function(row) {
                tableContent += row;
            });

            // Завершение таблицы
            tableContent += '</tbody></table>';

            // Вставка содержимого в div
            $('#soderzhimoe_colorboxa').html(tableContent);

            // Открытие Colorbox
            $.colorbox({
                inline: true,
                href: '#soderzhimoe_colorboxa',
                width: '80%',
                height: '80%',
                onComplete: function() {
                    $('#soderzhimoe_colorboxa').show(); // Явно показываем div после открытия модального окна
                },
                onClosed: function() {
                    $('#soderzhimoe_colorboxa').hide(); // Скрываем div, когда Colorbox закрывается
                }
            });
        });

        // Создание и вставка div #soderzhimoe_colorboxa в body
        if ($('#soderzhimoe_colorboxa').length === 0) {
            $('body').append('<div id=\"soderzhimoe_colorboxa\" style=\"display: none;\"></div>');
        }
    });
", 'inline');


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
        '#date_format' => 'Y-n-d',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );


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

    $form['row_index'] = array(
        '#type' => 'hidden',
        '#default_value' => $row_index,

    );

    $form['journal_id'] = array(
        '#type' => 'hidden',
        '#default_value' => $journal_id,

    );
    // Кнопка сабмита
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Сохранить'),
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    return $form;
}

function designer_journal_page_uchetniy_list_edit_form_submit($form, &$form_state) {
    // Получаем ID журнала и индекс строки
    $journal_id = $form_state['values']['journal_id'];
    $row_index = $form_state['values']['row_index'];

    // Загружаем ноду журнала
    $journal_node = node_load($journal_id);

    // Загружаем элемент коллекции полей
    $item = $journal_node->field_uchetniy_list['und'][$row_index];
    $field_collection_item = entity_load('field_collection_item', array($item['value']));
    $item = reset($field_collection_item);

    // Обработка текста с изображениями для поля "нарушения"
    $narushenija_text = $form_state['values']['field_list_narushenija']['value'];
    $narushenija_format = $form_state['values']['field_list_narushenija']['format'];

    // Ищем все теги <img> и оборачиваем их в ссылки на полные изображения (Colorbox) для поля "нарушения"
    $pattern = '/<img(.*?)src="(.*?)"(.*?)>/i';
    $replacement = '<a href="$2" class="colorbox" rel="gallery"><img$1src="$2"$3></a>';
    $text_with_colorbox_narushenija = preg_replace($pattern, $replacement, $narushenija_text);

    // Обработка текста с изображениями для поля "указания"
    $ukazanija_text = $form_state['values']['field_list_ukazanija']['value'];
    $ukazanija_format = $form_state['values']['field_list_ukazanija']['format'];

    // Ищем все теги <img> и оборачиваем их в ссылки на полные изображения (Colorbox) для поля "указания"
    $text_with_colorbox_ukazanija = preg_replace($pattern, $replacement, $ukazanija_text);

    // Устанавливаем значения для полей
    $item->field_list_date['und'][0]['value'] = $form_state['values']['field_list_date'];
    $item->field_list_narushenija['und'][0]['value'] = $text_with_colorbox_narushenija;
    $item->field_list_narushenija['und'][0]['format'] = $narushenija_format;
    $item->field_list_ukazanija['und'][0]['value'] = $text_with_colorbox_ukazanija;
    $item->field_list_ukazanija['und'][0]['format'] = $ukazanija_format;
    $item->field_list_oznak_org['und'][0]['value'] = $form_state['values']['field_list_oznak_org'];
    $item->field_list_oznak_zak['und'][0]['value'] = $form_state['values']['field_list_oznak_zak'];
    $item->field_list_otmetka_ukaz_proizv['und'][0]['value'] = $form_state['values']['field_list_otmetka_ukaz_proizv'];
    $item->field_list_otmetka_ukaz_predst['und'][0]['value'] = $form_state['values']['field_list_otmetka_ukaz_predst'];

    // Сохраняем элемент коллекции
    $item->save();

    // Добавляем данные в журнал изменений
    $field_collection_item_history = entity_create('field_collection_item', array('field_name' => 'field_history_uchetniy_list'));
    $field_collection_item_history->setHostEntity('node', $journal_node);

    // Заполняем поля историческими данными
    $field_collection_item_history->field_history_list_rowid['und'][0]['value'] = $row_index;
    $field_collection_item_history->field_history_list_date['und'][0]['value'] = $form_state['values']['field_list_date'];
    $field_collection_item_history->field_history_list_narushenija['und'][0]['value'] = $text_with_colorbox_narushenija;
    $field_collection_item_history->field_history_list_ukazanija['und'][0]['value'] = $text_with_colorbox_ukazanija;
    $field_collection_item_history->field_history_list_oznak_org['und'][0]['value'] = $form_state['values']['field_list_oznak_org'];
    $field_collection_item_history->field_history_list_oznak_zak['und'][0]['value'] = $form_state['values']['field_list_oznak_zak'];
    $field_collection_item_history->field_history_list_ukaz_proizv['und'][0]['value'] = $form_state['values']['field_list_otmetka_ukaz_proizv'];
    $field_collection_item_history->field_history_list_ukaz_predst['und'][0]['value'] = $form_state['values']['field_list_otmetka_ukaz_predst'];

    // Сохраняем элемент коллекции
    $field_collection_item_history->save();

    // Перезагружаем страницу после сохранения
    $form_state['redirect'] = $_SERVER['REQUEST_URI'];
}



