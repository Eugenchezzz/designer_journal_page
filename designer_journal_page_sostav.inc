<?php

/**
 * Обрабатывает страницу для пути 'objects/edit/%/journal_nadzora/sostav'.
 */
function designer_journal_page_sostav($object_id) {
    $index = 0;

    // сразу проверяем, кто запрашивает страницу, проектировщик и админ - ОК.
    global $user;
    if (!in_array('Проектировщик', array_values($user->roles)) && !in_array('administrator', array_values($user->roles))) {
        drupal_set_message('Доступ к журналу авторского надзора разрешен только для проектировщиков.', 'error');
        // Возвращаемся на предыдущую страницу
        drupal_goto($_SERVER['HTTP_REFERER']);
        return; // Прерываем выполнение текущей функции
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



    $header = array(

        t('ФИО'),
        t('Должность, место работы'),
        t('Работа, по которой осуществляется авторский надзор'),
        t('Дата и номер приказа о назначении'),
        t('Действие'),
    );



    $rows = array();

    if (isset($journal_node->field_sostav_rbochei['und']) && is_array($journal_node->field_sostav_rbochei['und'])) {
        foreach ($journal_node->field_sostav_rbochei['und'] as $item) {
            $field_collection_item = entity_load('field_collection_item', array($item['value']));
            if ($field_collection_item) {
                $item = reset($field_collection_item);

                // Основная строка с данными
                $rows[] = array(
                    'data' => array(
                        isset($item->field_fio['und'][0]['value']) ? check_plain($item->field_fio['und'][0]['value']) : '',
                        isset($item->field_dolzhnost['und'][0]['value']) ? check_plain($item->field_dolzhnost['und'][0]['value']) : '',
                        isset($item->field_jobname['und'][0]['value']) ? check_plain($item->field_jobname['und'][0]['value']) : '',
                        isset($item->field_datenum['und'][0]['value']) ? check_plain($item->field_datenum['und'][0]['value']) : '',
                        '<button class="open-history-button btn btn-default" data-index="' . $index . '" data-objectID="' . $object_id . '"> <span class="icon glyphicon glyphicon-time" title="История изменений" aria-hidden="true"></span> </button>
                    <button class="edit-button btn btn-default " data-index="' . $index . '"> <span class="icon glyphicon glyphicon-pencil" aria-hidden="true"></span> </button>',
                    ),
                    'id' => 'row-' . $index,
                );

                $form = drupal_get_form('designer_journal_page_sostav_edit_form', $item, $item->item_id, $index, $journal_id);

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
    }



    $form = drupal_get_form('designer_journal_sostav_rbochei_form', $journal_node);
    $rows[] = array(
        'data' => array(
            array(
                'data' => drupal_render($form),
                'colspan' => 6, // Объединяем все колонки
            ),
        ),
        'class' => array('edit-form-row'),
        'id' => 'edit-row-' . $index,// Скрываем форму по умолчанию
    );
    // Добавляем скрипт для обработки кнопки "Редактировать"
    drupal_add_js("
        jQuery(document).ready(function($) {
            $('.edit-button').click(function() {
                var index = $(this).data('index');
                $('#row-' + index).hide();
                $('#edit-row-' + index).show();
            });
        });
    ", 'inline');





    // ОБРАБОТЧИК ОТКРЫТИЯ ЖУРНАЛА (полностью тут)

    $history_items = [];

    if (isset($journal_node->field_history_sostav['und']) && is_array($journal_node->field_history_sostav['und'])) {
        foreach ($journal_node->field_history_sostav['und'] as $item) {
            $field_collection_item = entity_load('field_collection_item', array($item['value']));
            $history_item = reset($field_collection_item);

            $history_items[] = [
                'rowid' => isset($history_item->field_history_sostav_rowid['und'][0]['value']) ? $history_item->field_history_sostav_rowid['und'][0]['value'] : '',
                'fio' => isset($history_item->field_history_fio['und'][0]['value']) ? $history_item->field_history_fio['und'][0]['value'] : '',
                'dolzhnost' => isset($history_item->field_history_dolzhnost['und'][0]['value']) ? $history_item->field_history_dolzhnost['und'][0]['value'] : '',
                'jobname' => isset($history_item->field_history_jobname['und'][0]['value']) ? $history_item->field_history_jobname['und'][0]['value'] : '',
                'datenum' => isset($history_item->field_history_datenum['und'][0]['value']) ? $history_item->field_history_datenum['und'][0]['value'] : '',
                'author' => isset($history_item->field_history_author['und'][0]['value']) ? $history_item->field_history_author['und'][0]['value'] : '',
                'change_date' => isset($history_item->field_history_change_date['und'][0]['value']) ? $history_item->field_history_change_date['und'][0]['value'] : '',
            ];
        }
    }
    $json_history_data = json_encode($history_items);

    drupal_add_js("
    jQuery(document).ready(function($) {
        // Данные истории в JSON формате
        var historyData = " . $json_history_data . ";

        // Обработчик клика на кнопку
        $('.open-history-button').click(function() {
            var index = $(this).data('index').toString(); // Преобразование индекса в строку

            var tableContent = '<table class=\"table\"><thead><tr>' +
                '<th>ФИО</th>' +
                '<th>Занимаемая должность</th>' +
                '<th>Наименование работ</th>' +
                '<th>Дата и номер приказа</th>' +
                '<th>Дата изменения</th>' +
            '</tr></thead><tbody>';

            var rows = [];

            // Фильтрация данных по значению индекса и добавление строк в массив



$.each(historyData, function(i, item) {
    var rowidStr = item.rowid.toString(); // Преобразование rowid в строку
    var match = (rowidStr === index) ? 'Match' : 'No match'; // Результат сравнения


    var changeDate = new Date(item.change_date);
    var formattedChangeDate = ('0' + changeDate.getDate()).slice(-2) + '.' +
                              ('0' + (changeDate.getMonth() + 1)).slice(-2) + '.' +
                              changeDate.getFullYear();

    // Добавление строки в массив только если есть совпадение
    if (match === 'Match') {
        rows.push(
            '<tr>' +
            '<td>' + item.fio + '</td>' +
            '<td>' + item.dolzhnost + '</td>' +
            '<td>' + item.jobname + '</td>' +
            '<td>' + item.datenum + '</td>' + // Оставляем дату без изменений
            '<td>' + formattedChangeDate + '</td>' + // Используем отформатированную дату изменения
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



























    drupal_add_js("
document.querySelectorAll(\"button[id^='edit-submit']\").forEach((btn, index, buttons) => {
  if (index !== buttons.length - 1) {
    btn.classList.add('highlight');
  }
});
", 'inline');


    //ЗДЕСЬ ВСЕ ОСНОВНЫЕ СТИЛИ
    $output .= '<style>
/* Стиль кнопок сабмита редактирования */

.form-submit:not(#designer-journal-sostav-rbochei-form div .form-submit) {
    font-size: 0; /* Скрывает текст */
    
    min-width: 40px !important;
    min-height: 34px !important;
    margin: 0 !important;
    padding: 0 !important;
    border: solid 1px #cacaca;
    border-radius: 4px;
    background-color: #fdfdfd; /* Цвет фона */

}


.form-submit:not(#designer-journal-sostav-rbochei-form div .form-submit) span {
    font-size: initial; /* Восстанавливает размер текста внутри span */
    color: #333333;
    position: relative;
    left: 4px;
    margin-bottom: 4px ;
}

.form-submit:not(#designer-journal-sostav-rbochei-form div .form-submit):hover span {
    color: #ffffff; /* Цвет текста при наведении */
}



.fields-inline .open-history-button {
    margin-left: 2%; /* Добавляет отступ перед вторым последним элементом, если нужно */
    margin-right: 0.2%;
}





/* ПЕРЕСТАНОВКА КНОПОЧКИ В КОЛОРБОКСЕ */
#cboxClose{
    position: absolute !important;
    top: 1px !important;
}



table tr td:last-child {
      white-space: nowrap;
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



.form-item-field-fio{
    left: 0 !important;
}

    .form-item {
        margin: 10px !important;
        padding: 0px !important;
        flex: 1 !important; /* Элементы будут занимать пространство в зависимости от установленного flex-basis */
    }

    .form-item-field-fio {
        flex: 0 0 22% !important; /* Ширина для field_fio */

    }

    .form-item-field-dolzhnost {
        flex: 0 22% !important; /* Ширина для field_dolzhnost */
    }

    .form-item-field-jobname {
        flex: 0 0 22% !important; /* Ширина для field_jobname */
    }

    .form-item-field-datenum {
        flex: 0 0 22% !important; /* Ширина для field_datenum */
    }

    .form-item input {
        width: 100% !important; /* Поле ввода занимает всю ширину своего контейнера */
        box-sizing: border-box !important; /* Учитываем padding и border в ширину элемента */
        margin-right: 0 !important; /* Убираем отступы справа */
    }

    .container {
        display: flex !important; /* Используем flexbox для расположения дочерних элементов */
    }

     
.table-responsive {
    width: 100% !important;
}

/* Стилизация таблицы */
.table th, .table td {
     /* Граница ячейки таблицы */
    padding: 8px !important; /* Отступ внутри ячейки */
    text-align: left !important;
}

/* Обрезка и перенос текста */
.table td {
    width: 400px !important; /* Статическая ширина ячейки */
    overflow: hidden !important;
    text-overflow: ellipsis !important; /* Добавляет троеточие для длинного текста */
    word-break: break-word !important; /* Переносит длинные слова */
}

/* Ширина ячеек последнего столбца */
.table td:last-child {
    width: 5px !important; /* Статическая ширина для последнего столбца */
}

#block-system-main {
    width: 100% !important;
}

 /* ПРИДАНИЕ СТИЛЕЙ КАК У ВЛАДА ТАБЛИЧКАМ */
.table.table-hover.table-striped {
    min-width: 1200px !important;
}

 .table.table-hover.table-striped tbody tr:nth-child(4n-3), /* 1 и 2 строки */
.table.table-hover.table-striped tbody tr:nth-child(4n-2) {
    background-color: #f2f7fd;
}

.table.table-hover.table-striped tbody tr:nth-child(4n-1), /* 3 и 4 строки */
.table.table-hover.table-striped tbody tr:nth-child(4n) {
    background-color: #eaeff5; /* Серый цвет, можно поменять */
}

</style>';

    $output .= theme('table', array('header' => $header, 'rows' => $rows)); // рендер таблицы

    // $form = drupal_get_form('designer_journal_sostav_rbochei_form', $journal_node);
    // $output .= drupal_render($form);
    return $output;
}



//ФОРМА ДОБАВЛЕНИЯ


function designer_journal_sostav_rbochei_form($form, &$form_state, $journal_node) {
    // Поля формы и кнопка submit
    $form['fields_container'] = array(
        '#type' => 'container',
        '#attributes' => array(
            'class' => array('fields-inline', 'form-inline'),
            'style' => 'display: flex;  align-items: center;  gap: 5px', // Flex-wrap для переноса на новую строку при нехватке места
        ),
    );

    $form['fields_container']['field_fio'] = array(
        '#type' => 'textfield',
//        '#title' => 'ФИО',
        '#required' => TRUE,
        '#default_value' => '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['fields_container']['field_dolzhnost'] = array(
        '#type' => 'textfield',
//        '#title' => 'Должность, место работы',
        '#required' => TRUE,
        '#default_value' => '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['fields_container']['field_jobname'] = array(
        '#type' => 'textfield',
//        '#title' => 'Работа, по которой осуществляется надзор',
        '#required' => TRUE,
        '#default_value' => '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['fields_container']['field_datenum'] = array(
        '#type' => 'textfield',
//        '#title' => 'Дата и номер приказа',
        '#required' => TRUE,
        '#default_value' => '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px; ',
        ),
    );

    // Кнопка для сохранения формы
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Добавить'),
        '#attributes' => array(
            'class' => array('form-submit-inline'),
            'style' => 'align-self: center !important; margin: 0 !important', // Кнопка занимает 20% ширины контейнера
        ),
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




















// ДИЧЬ ДЛЯ РЕДАКТИРОВАНИЯ ЗАПИСИ

function designer_journal_page_sostav_edit_form($form, &$form_state, $item, $item_id, $index, $journal_id) {

    $form['container'] = array(
        '#type' => 'container',
        '#attributes' => array(
            'class' => array('fields-inline', 'form-inline'),
            'style' => 'display: flex;  align-items: center;  gap: 1px !important;', // Flex-wrap для переноса на новую строку при нехватке места
        ),
    );


    $form['container']['field_fio'] = array(
        '#type' => 'textfield',

        '#default_value' => isset($item->field_fio['und'][0]['value']) ? $item->field_fio['und'][0]['value'] : '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['container']['field_dolzhnost'] = array(
        '#type' => 'textfield',

        '#default_value' => isset($item->field_dolzhnost['und'][0]['value']) ? $item->field_dolzhnost['und'][0]['value'] : '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );

    $form['container']['field_jobname'] = array(
        '#type' => 'textfield',

        '#default_value' => isset($item->field_jobname['und'][0]['value']) ? $item->field_jobname['und'][0]['value'] : '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px;',
        ),
    );
// СЮДА ДОПИШЕМ СТИЛИ ЧТОБЫ БЫЛ МАКСИМАЛЬНЫЙ ОТСТУП
    $form['container']['field_datenum'] = array(
        '#type' => 'textfield',

        '#default_value' => isset($item->field_datenum['und'][0]['value']) ? $item->field_datenum['und'][0]['value'] : '',
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 1px;',
        ),
    );

    $form['container']['item_id'] = array(

        '#type' => 'hidden',
        '#default_value' => $item_id,
    );

    $form['container']['index'] = array(

        '#type' => 'hidden',
        '#default_value' => $index,
    );
    $form['container']['journal_id'] = array(

        '#type' => 'hidden',
        '#default_value' => $journal_id,
    );

    $form['container']['open_history_button'] = array(
        '#type' => 'markup',
        '#markup' => '<button class="open-history-button btn btn-default" data-index="' . $index . '" data-objectID="' . $item_id . '">
                    <span class="icon glyphicon glyphicon-time" title="История изменений" aria-hidden="true"></span>
                  </button>',
        '#attributes' => array(
            'class' => array('form-field-inline'),

        ),
    );


    $form['container']['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Save'),
        '#attributes' => array(
            'class' => array('form-field-inline'),
            'style' => 'margin-right: 10px; ',
        ),
    );






    return $form;
}


function designer_journal_page_sostav_edit_form_submit($form, &$form_state) {
    global $user;

    // Загружаем объект field_collection_item по ID
    $item_id = $form_state['values']['item_id'];
    $field_collection_item = entity_load('field_collection_item', array($item_id));

    if (!$field_collection_item) {
        drupal_set_message(t('Не удалось загрузить элемент коллекции полей с ID: @item_id', array('@item_id' => $item_id)), 'error');
        return;
    }

    $item = reset($field_collection_item);

    // Получение новых данных из формы
    $field_datenum = $form_state['values']['field_datenum'];
    $field_dolzhnost = $form_state['values']['field_dolzhnost'];
    $field_jobname = $form_state['values']['field_jobname'];
    $field_fio = $form_state['values']['field_fio'];
    $journal_id = $form_state['values']['journal_id'];

    // Загрузка ноды журнала
    $journal_node = node_load($journal_id);

    if ($journal_node) {
        // Создание нового элемента в коллекции field_history_sostav
        $field_collection_item_history = entity_create('field_collection_item', array('field_name' => 'field_history_sostav'));

        // Устанавливаем значения для полей
        $field_collection_item_history->setHostEntity('node', $journal_node);
        $field_collection_item_history->field_history_fio['und'][0]['value'] = $field_fio;
        $field_collection_item_history->field_history_dolzhnost['und'][0]['value'] = $field_dolzhnost;
        $field_collection_item_history->field_history_jobname['und'][0]['value'] = $field_jobname;
        $field_collection_item_history->field_history_datenum['und'][0]['value'] = $field_datenum;
        $field_collection_item_history->field_history_author['und'][0]['value'] = $user->name; // Логин текущего пользователя
        $field_collection_item_history->field_history_change_date['und'][0]['value'] = date('Y-m-d H:i:s'); // Текущая дата и время
        $field_collection_item_history->field_history_sostav_rowid['und'][0]['value'] = $form_state['values']['index'];

        // Сохраняем элемент коллекции
        $field_collection_item_history->save();

        // Добавляем сообщение о успешном добавлении
        drupal_set_message(t('Запись о предыдущем варианте успешно добавлена в журнал.'));
    } else {
        drupal_set_message(t('Не удалось загрузить ноду журнала с ID: @journal_id', array('@journal_id' => $journal_id)), 'error');
    }

    // Обновление элемента коллекции полей
    $item->field_fio['und'][0]['value'] = $field_fio;
    $item->field_dolzhnost['und'][0]['value'] = $field_dolzhnost;
    $item->field_jobname['und'][0]['value'] = $field_jobname;
    $item->field_datenum['und'][0]['value'] = $field_datenum;
    $item->save();

    drupal_set_message(t('Изменения успешно сохранены.'));
    $form_state['redirect'] = $_SERVER['REQUEST_URI'];
}
