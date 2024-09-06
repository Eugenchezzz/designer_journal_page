<?php

/**
 * Обрабатывает страницу для пути 'objects/edit/%/journal_nadzora/sostav'.
 */
function designer_journal_page_nadzor_predstaviteli($object_id) {
    // сразу проверяем, кто запрашивает страницу, проектировщик и админ - ОК.
    global $user;
    if (!in_array('Проектировщик', array_values($user->roles)) && !in_array('administrator', array_values($user->roles))) {
        drupal_set_message('Доступ к журналу авторского надзора разрешен только для проектировщиков.', 'error');
        // Возвращаемся на предыдущую страницу
        drupal_goto($_SERVER['HTTP_REFERER']);
        return; // Прерываем выполнение текущей функции
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




  //ВЫВОД ДАННЫХ В ТАБЛИЦЕ
  // Start building the table
  $header = array(
    t('Фамилия, инициалы'),
    t('Место работы'),
    t('Занимаемая должность'),
    t('Номер телефона'),
    t('Дата приезда'),
    t('Дата отъезда'),
    t('Действие'),
  );

  $rows = array();

  // цикл записи в табличку для отображения
    $index = 0;
  foreach ($journal_node->field_predstaviteli_nadzora['und'] as $item) {
    $field_collection_item = entity_load('field_collection_item', array($item['value']));
    if ($field_collection_item) {
      $item = reset($field_collection_item);


        // Форматируем дату из поля field_reg_start_date
        $start_date = isset($item->field_reg_start_date['und'][0]['value']) ? $item->field_reg_start_date['und'][0]['value'] : '';
        $formatted_start_date = $start_date ? date('Y-m-d', strtotime($start_date)) : '';

// Форматируем дату из поля field_reg_end_date
        $end_date = isset($item->field_reg_end_date['und'][0]['value']) ? $item->field_reg_end_date['und'][0]['value'] : '';
        $formatted_end_date = $end_date ? date('Y-m-d', strtotime($end_date)) : '';


      $rows[] = array(
          'data' => array(
           isset($item->field_reg_fio['und'][0]['value']) ? check_plain($item->field_reg_fio['und'][0]['value']) : '',
           isset($item->field_reg_place['und'][0]['value']) ? check_plain($item->field_reg_place['und'][0]['value']) : '',
           isset($item->field_reg_dolzhnost['und'][0]['value']) ? check_plain($item->field_reg_dolzhnost['und'][0]['value']) : '',
           isset($item->field_reg_phone['und'][0]['value']) ? check_plain($item->field_reg_phone['und'][0]['value']) : '',
           $formatted_start_date,
           $formatted_end_date,
              '<button class="open-history-button btn btn-default form-submit" data-index="' . $index . '" data-objectID="' . $object_id . '"> <span class="icon glyphicon glyphicon-time" title="История изменений" aria-hidden="true"></span> </button>
              <button class="edit-button btn btn-default form-submit" data-index="' . $index . '"> <span class="icon glyphicon glyphicon-pencil" aria-hidden="true"></span> </button>',
       ),
          'id' => 'row-' . $index,
      );

        $form = drupal_get_form('designer_journal_page_nadzor_edit_form', $item, $item->item_id, $index, $journal_id);
        // Скрытая строка с формой
        $rows[] = array(
            'data' => array(
                array(
                    'data' => drupal_render($form),
                    'colspan' => 7, // Объединяем все колонки
                ),
            ),
            'class' => array('edit-form-row'),
            'id' => 'edit-row-' . $index,
            'style' => 'display: none;', // Скрываем форму по умолчанию
        );


    }
    $index++;
      drupal_add_js("
        jQuery(document).ready(function($) {
            $('.edit-button').click(function() {
                var index = $(this).data('index');
                $('#row-' + index).hide();
                $('#edit-row-' + index).show();
            });
        });
    ", 'inline');


  }

  //ПОЛНАЯ ОБРАБОТКА ЖУРНАЛА ИСТОРИИ ИЗМЕНЕНИЙ
    $history_items = [];

    foreach ($journal_node->field_history_nadzor['und'] as $item) {
        $field_collection_item = entity_load('field_collection_item', array($item['value']));
        $history_item = reset($field_collection_item);

        $history_items[] = [
            'rowid' => isset($history_item->field_history_rowid['und'][0]['value']) ? $history_item->field_history_rowid['und'][0]['value'] : '',
            'reg_fio' => isset($history_item->field_history_reg_fio['und'][0]['value']) ? $history_item->field_history_reg_fio['und'][0]['value'] : '',
            'reg_place' => isset($history_item->field_history_reg_place['und'][0]['value']) ? $history_item->field_history_reg_place['und'][0]['value'] : '',
            'reg_dolzhnost' => isset($history_item->field_history_reg_dolzhnost['und'][0]['value']) ? $history_item->field_history_reg_dolzhnost['und'][0]['value'] : '',
            'reg_phone' => isset($history_item->field_history_reg_phone['und'][0]['value']) ? $history_item->field_history_reg_phone['und'][0]['value'] : '',
            'reg_start_date' => isset($history_item->field_history_reg_start_date['und'][0]['value']) ? $history_item->field_history_reg_start_date['und'][0]['value'] : '',
            'reg_end_date' => isset($history_item->field_history_reg_end_date['und'][0]['value']) ? $history_item->field_history_reg_end_date['und'][0]['value'] : '',
            'reg_author' => isset($history_item->field_history_reg_author['und'][0]['value']) ? $history_item->field_history_reg_author['und'][0]['value'] : '',
            'reg_change_date' => isset($history_item->field_history_reg_change_date['und'][0]['value']) ? $history_item->field_history_reg_change_date['und'][0]['value'] : '',
        ];

    }
    $json_history_data = json_encode($history_items);

    drupal_add_js("
    jQuery(document).ready(function($) {
        // Данные истории в JSON формате
        var historyData = " . $json_history_data . ";

        // Обработчик клика на кнопку
        $('.open-history-button').click(function() {
            var index = $(this).data('index').toString(); // Преобразование индекса в строку

            var tableContent = '<h2>Журнал</h2><table class=\"table\"><thead><tr>' +
                '<th>FIO</th>' +
                '<th>Place</th>' +
                '<th>Position</th>' +
                '<th>Phone</th>' +
                '<th>Start Date</th>' +
                '<th>End Date</th>' +
                '<th>Author</th>' +
                '<th>Change Date</th>' +
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
                        '<td>' + item.reg_fio + '</td>' +
                        '<td>' + item.reg_place + '</td>' +
                        '<td>' + item.reg_dolzhnost + '</td>' +
                        '<td>' + item.reg_phone + '</td>' +
                        '<td>' + item.reg_start_date + '</td>' +
                        '<td>' + item.reg_end_date + '</td>' +
                        '<td>' + item.reg_author + '</td>' +
                        '<td>' + item.reg_change_date + '</td>' +
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




//ОБРАБОТКА ЖУРНАЛА ИЗМЕНЕНИЙ ЗАКОНЧЕНА


  // Render the table
  $output .= theme('table', array('header' => $header, 'rows' => $rows));

    $output .= '<style>

/* ПЕРЕСТАНОВКА КНОПОЧКИ В КОЛОРБОКСЕ */
#cboxClose{
    position: absolute !important;
    top: 1px !important;
}


.edit-form-row td {
    padding: 0;
    margin: 0;
}

.edit-form-row form {
    width: 100%;
    display: flex;
    align-items: center;
}







    .form-item {
        margin: 0 !important;
        padding: 0 !important;
        flex: 1 !important; /* Элементы будут занимать пространство в зависимости от установленного flex-basis */
    }

    .form-item-field-reg-fio {
    
        flex: 0 0 260px !important; /* Ширина для field_fio */
    }

    .form-item-field-reg-place{
        flex: 0 0 260px !important; /* Ширина для field_dolzhnost */
    }

    .form-item-field-reg-dolzhnost {
        flex: 0 0 260px !important; /* Ширина для field_jobname */
        
    }

    .form-item-field-reg-phone {
        flex: 0 0 250px !important; /* Ширина для field_datenum */
    }
    .form-item-field-reg-start-date {
        flex: 0 0 2px !important; /* Ширина для field_datenum */
        width: 260px !important;
    }
        .form-item-field-reg-end-date {
        flex: 0 0 2px !important; /* Ширина для field_datenum */
        width: 260px !important;
    }

    .form-item input {
        width: 100% !important; /* Поле ввода занимает всю ширину своего контейнера */
        box-sizing: border-box !important; /* Учитываем padding и border в ширину элемента */
        margin-right: 0 !important; /* Убираем отступы справа */
        
    }

    .container {
        display: flex !important; /* Используем flexbox для расположения дочерних элементов */
    }
    #edit-fields-container {
    left: 10px !important;
    }
     
    .table-responsive{
width: 100% !important;
}

/* Стилизация таблицы */


.table th, .table-custom td {
   
    padding: 5px !important; /* Отступ внутри ячейки */
    text-align: left !important;
}

/* Обрезка и перенос текста */
.table td {
    width: 250px !important; /* Максимальная ширина ячейки, можете настроить */
    overflow: hidden !important;
    text-overflow: ellipsis !important; /* Добавляет троеточие для длинного текста */
   /* white-space: nowrap !important; /* Не переносит текст на новую строку */
    word-break: break-word !important; /* Переносит длинные слова */
}
.table td:last-child {
    width: 5px !important; /* Статическая ширина для последнего столбца */
}
    
</style>';
  $form = drupal_get_form('designer_journal_predstaviteli_nadzora_form', $journal_node);
  $output .= drupal_render($form);






  return $output;
}



// ФОРМА ДОБАВЛЕНИЯ ПРЕДСТАВИТЕЛЯ НАДЗОРА

/**
 * Функция для создания формы добавления данных в коллекцию полей field_predstaviteli_nadzora.
 */
function designer_journal_predstaviteli_nadzora_form($form, &$form_state, $journal_node) {

    $form['fields_container'] = array(
        '#type' => 'container',
        '#attributes' => array(
            'class' => array('fields-inline', 'form-inline'),
            'style' => 'display: flex;  align-items: center;  gap: 5px', // Flex-wrap для переноса на новую строку при нехватке места
        ),
    );
  // Создаем форму с пустыми значениями по умолчанию
  $form['fields_container']['field_reg_fio'] = array(
    '#type' => 'textfield',
//    '#title' => t('ФИО'),
    '#default_value' => '',
  );

  $form['fields_container']['field_reg_place'] = array(
    '#type' => 'textfield',
//    '#title' => t('Место'),
    '#default_value' => '',
  );

  $form['fields_container']['field_reg_dolzhnost'] = array(
    '#type' => 'textfield',
//    '#title' => t('Должность'),
    '#default_value' => '',
  );

  $form['fields_container']['field_reg_phone'] = array(
    '#type' => 'textfield',
//    '#title' => t('Телефон'),
    '#default_value' => '',
  );





    $form['fields_container']['field_reg_start_date'] = array(
        '#type' => 'date_popup',
//        '#title' => t('Дата начала'),
        '#default_value' => !empty($field_collection_item->field_reg_start_date['und'][0]['value'])
            ? array(
                'year' => date('Y', strtotime($field_collection_item->field_reg_start_date['und'][0]['value'])),
                'month' => date('n', strtotime($field_collection_item->field_reg_start_date['und'][0]['value'])),
                'day' => date('j', strtotime($field_collection_item->field_reg_start_date['und'][0]['value']))
            )
            : array('year' => '', 'month' => '', 'day' => ''),
        '#date_date_element' => 'date',
        '#date_month_element' => 'month',
        '#date_year_element' => 'year',
        '#date_day_element' => 'day',
        '#date_format' => 'Y-m-d', // Это должно убрать поле времени
    );

    $form['fields_container']['field_reg_end_date'] = array(
        '#type' => 'date_popup',
//        '#title' => t('Дата окончания'),
        '#default_value' => !empty($field_collection_item->field_reg_end_date['und'][0]['value'])
            ? array(
                'year' => date('Y', strtotime($field_collection_item->field_reg_end_date['und'][0]['value'])),
                'month' => date('n', strtotime($field_collection_item->field_reg_end_date['und'][0]['value'])),
                'day' => date('j', strtotime($field_collection_item->field_reg_end_date['und'][0]['value']))
            )
            : array('year' => '', 'month' => '', 'day' => ''),
        '#date_date_element' => 'date',
        '#date_month_element' => 'month',
        '#date_year_element' => 'year',
        '#date_day_element' => 'day',
        '#date_format' => 'Y-m-d', // Это должно убрать поле времени
    );









    $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Добавить'),
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

  //обработка дат
    // Получаем значения из формы
    $start_date = isset($form_state['values']['field_reg_start_date']) ? $form_state['values']['field_reg_start_date'] : '';
    $end_date = isset($form_state['values']['field_reg_end_date']) ? $form_state['values']['field_reg_end_date'] : '';

    // Проверяем наличие обеих дат
    if (empty($start_date) || empty($end_date)) {
        // Устанавливаем ошибку в форме
        if (empty($start_date)) {
            form_set_error('field_reg_start_date', t('Поле "Дата начала" обязательно для заполнения.'));
        }
        if (empty($end_date)) {
            form_set_error('field_reg_end_date', t('Поле "Дата окончания" обязательно для заполнения.'));
        }

        // Возвращаем пользователя на ту же страницу с формой
        $form_state['rebuild'] = TRUE;
    } else {
        // Если обе даты присутствуют, сохраняем данные
        $field_collection_item->field_reg_start_date['und'][0]['value'] = $start_date;
        $field_collection_item->field_reg_end_date['und'][0]['value'] = $end_date;

        // Вы можете выполнить дополнительные действия, если необходимо
        // Например, сохранение данных или редирект
    }





    // Сохраняем элемент коллекции
  $field_collection_item->save();



  // Отправляем сообщение об успешном добавлении
  drupal_set_message(t('Представитель успешно добавлен!'));

  // Перезагружаем текущую страницу
  $form_state['redirect'] = $_GET['q'];
}


// ОБРАБОТЧИКИ РЕДАКТИРОВАНИЯ ДАННЫХ




function designer_journal_page_nadzor_edit_form($form, &$form_state, $item, $item_id, $index, $journal_id){
    $form['container'] = array(
        '#type' => 'container',
        '#attributes' => array(
            'class' => array('fields-inline', 'form-inline'),
            'style' => 'display: flex; align-items: center; gap: 20px;', // Flex-wrap для переноса на новую строку при нехватке места
        ),
    );

    $form['container']['field_reg_fio'] = array(
        '#type' => 'textfield',
        '#default_value' => isset($item->field_reg_fio['und'][0]['value']) ? $item->field_reg_fio['und'][0]['value'] : '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['container']['field_reg_place'] = array(
        '#type' => 'textfield',
        '#default_value' => isset($item->field_reg_place['und'][0]['value']) ? $item->field_reg_place['und'][0]['value'] : '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['container']['field_reg_dolzhnost'] = array(
        '#type' => 'textfield',
        '#default_value' => isset($item->field_reg_dolzhnost['und'][0]['value']) ? $item->field_reg_dolzhnost['und'][0]['value'] : '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['container']['field_reg_phone'] = array(
        '#type' => 'textfield',
        '#default_value' => isset($item->field_reg_phone['und'][0]['value']) ? $item->field_reg_phone['und'][0]['value'] : '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['container']['field_reg_start_date'] = array(
        '#type' => 'date_popup',

        '#default_value' => !empty($item->field_reg_start_date['und'][0]['value'])
            ? array(
                'year' => date('Y', strtotime($item->field_reg_start_date['und'][0]['value'])),
                'month' => date('n', strtotime($item->field_reg_start_date['und'][0]['value'])),
                'day' => date('j', strtotime($item->field_reg_start_date['und'][0]['value']))
            )
            : array('year' => '', 'month' => '', 'day' => ''),
        '#date_date_element' => 'date',
        '#date_month_element' => 'month',
        '#date_year_element' => 'year',
        '#date_day_element' => 'day',
        '#date_format' => 'Y-m-d', // Это убирает поле времени
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['container']['field_reg_end_date'] = array(
        '#type' => 'date_popup',

        '#default_value' => !empty($item->field_reg_end_date['und'][0]['value'])
            ? array(
                'year' => date('Y', strtotime($item->field_reg_end_date['und'][0]['value'])),
                'month' => date('n', strtotime($item->field_reg_end_date['und'][0]['value'])),
                'day' => date('j', strtotime($item->field_reg_end_date['und'][0]['value']))
            )
            : array('year' => '', 'month' => '', 'day' => ''),
        '#date_date_element' => 'date',
        '#date_month_element' => 'month',
        '#date_year_element' => 'year',
        '#date_day_element' => 'day',
        '#date_format' => 'Y-m-d', // Это убирает поле времени
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['container']['item_id'] = array(
        '#type' => 'hidden',
        '#default_value' => $item_id,
    );

    $form['container']['journal_id'] = array(
        '#type' => 'hidden',
        '#default_value' => $journal_id,
    );
    $form['container']['index'] = array(
        '#type' => 'hidden',
        '#default_value' => $index,
    );

    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
    );

    return $form;
}

function designer_journal_page_nadzor_edit_form_submit($form, &$form_state)
{
    // создадим запись в журнале изменений
    global $user;

    // Загружаем элемент коллекции по item_id
    $item_id = $form_state['values']['item_id'];
    $field_collection_item = entity_load('field_collection_item', array($item_id));
    $item = reset($field_collection_item);


    // достанем предыдущие данные, которые будем заменять, и запихнем их в новую запись в журнале истории изменений

    $rowid = $form_state['values']['index'];
    $old_field_reg_fio = isset($item->field_reg_fio['und'][0]['value']) ? $item->field_reg_fio['und'][0]['value'] : '';
    $old_field_reg_place = isset($item->field_reg_place['und'][0]['value']) ? $item->field_reg_place['und'][0]['value'] : '';
    $old_field_reg_dolzhnost = isset($item->field_reg_dolzhnost['und'][0]['value']) ? $item->field_reg_dolzhnost['und'][0]['value'] : '';
    $old_field_reg_phone = isset($item->field_reg_phone['und'][0]['value']) ? $item->field_reg_phone['und'][0]['value'] : '';
    $old_field_start_date = isset($item->field_reg_start_date['und'][0]['value']) ? $item->field_reg_start_date['und'][0]['value'] : '';
    $old_field_end_date = isset($item->field_reg_end_date['und'][0]['value']) ? $item->field_reg_end_date['und'][0]['value'] : '';

    //сохраняем новые данные
    $journal_id = $form_state['values']['journal_id'];
    $journal_node = node_load($journal_id);

    if ($journal_node) {
        // Создание нового элемента в коллекции field_history_nadzor
        $field_collection_item_history = entity_create('field_collection_item', array('field_name' => 'field_history_nadzor'));

        // Устанавливаем значения для полей
        $field_collection_item_history->setHostEntity('node', $journal_node);
        $field_collection_item_history->field_history_reg_fio['und'][0]['value'] = $old_field_reg_fio;
        $field_collection_item_history->field_history_reg_dolzhnost['und'][0]['value'] = $old_field_reg_dolzhnost;
        $field_collection_item_history->field_history_reg_place['und'][0]['value'] = $old_field_reg_place;
        $field_collection_item_history->field_history_reg_phone['und'][0]['value'] = $old_field_reg_phone;
        $field_collection_item_history->field_history_reg_start_date['und'][0]['value'] = $old_field_start_date;
        $field_collection_item_history->field_history_reg_end_date['und'][0]['value'] = $old_field_end_date;
        $field_collection_item_history->field_history_reg_author['und'][0]['value'] = $user->name; // Логин текущего пользователя
        $field_collection_item_history->field_history_reg_change_date['und'][0]['value'] = date('Y-m-d H:i:s'); // Текущая дата и время
        $field_collection_item_history->field_history_rowid['und'][0]['value'] = $rowid;

        // Сохраняем элемент коллекции
        $field_collection_item_history->save();
        // Добавляем сообщение о успешном добавлении
        drupal_set_message(t('Запись о предыдущем варианте успешно добавлена в журнал.'));
    }else {
        drupal_set_message(t('Не удалось загрузить ноду журнала с ID: @journal_id', array('@journal_id' => $journal_id)), 'error');
    }





    if ($field_collection_item) {
        $item = reset($field_collection_item);

        // Обновляем значения полей в элементе коллекции
        $item->field_reg_fio['und'][0]['value'] = $form_state['values']['field_reg_fio'];
        $item->field_reg_place['und'][0]['value'] = $form_state['values']['field_reg_place'];
        $item->field_reg_dolzhnost['und'][0]['value'] = $form_state['values']['field_reg_dolzhnost'];
        $item->field_reg_phone['und'][0]['value'] = $form_state['values']['field_reg_phone'];
        $item->field_reg_start_date['und'][0]['value'] = $form_state['values']['field_reg_start_date'];
        $item->field_reg_end_date['und'][0]['value'] = $form_state['values']['field_reg_end_date'];

        // Сохраняем изменения
        $item->save();
    }

    // Устанавливаем сообщение об успешном сохранении
    drupal_set_message(t('Данные успешно обновлены.'));

    // Перенаправляем на нужную страницу (например, на текущую страницу
}