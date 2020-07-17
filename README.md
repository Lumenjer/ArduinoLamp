﻿# ArduinoLamp 

Это облегченная версия прошивки AlexGyver под ардуино.
(Оригинал здесь: ![Исходная версия](https://github.com/AlexGyver/GyverLamp/))

Обновления от:https://github.com/Norovl/GyverLamp_for_Arduino

## Краткое описание:

Лампа на адресных светодиодах, позволяющих получить красивые эффекты свечения:

Из оригинала было убрано:
- 	будильник;
- 	работа с Wi-Fi (у ардуинки его нет);
- 	эффект Светляки - под него не хватило места в ОЗУ;

Что добавлено (как ни странно, но да, было что добавить):
-   эффект Радуга диагональная и Звездопад (версия 1.1), Метель и Горящий шум(сделан мной) (1.31) Мячики(Убран), Светлячки со шлефом, Блуждающий кубик, Водопад(Простой и 4в1), Мерцание(Убран), Синусоид, Метаболз, Кодовый замок(Убран), Шум дыма(сделан мной), Шумовая волна(этот эффект сделан мной и Сотнегом)(версия old_button)Разноцветный дождь(1.4);
- 	эффект Светляки заменен на Белая лампа;
-	демо включается или выключается если нажать 4 раз подряд;      
- 	сохранение настроек всех эффектов, в т.ч. текущего режима в энергонезависимой
	памяти.	Производится пятикратным нажатием кнопки, подтверждением будет
	выключение и включение лампы;
- 	регулирование скорости (speed) эффектов путем двойного нажатия и удержания
	кнопки на втором нажатии;
- 	регулирование масштаба (scale) эффектов путем тройного нажатия и удержания 
	кнопки на третьем нажатии;
- 	индикация уровня яркости/скорости/масштаба вертикальной полосой	оранжевого/
	зеленого/синего цветов соответственно;

Регулировка уровня яркости/скорости/масштаба реверсивная, т.е. при повторном
регулировании изменения будут производиться в обратную сторону (сначала в бОльшую,
затем в мЕньшую).

Для работы всех режимов кнопки ОБЯЗАТЕЛЬНО нужно заменить у себя папку GyverButton
на аналогичную с этого сайта. Совместимость с оригиналом сохранилась.
## Внимание:
На матрице 16 на 16 оперативной пямяти не хватает: или матрица 8 на 8, или выключаем эффекты;

Брать только 1.1 или old_button или 1.4 (с новою прошывкою от Norvol нормально кнопка не работала, и здесь тоже)
