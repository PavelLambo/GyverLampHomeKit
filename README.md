# GyverLampHomeKit

Оригинальный проект - https://alexgyver.ru/gyverlamp/

Добавил поддержку Apple HomeKIT в GyverLamp. Можно включать/Выключать, менять яркость и режимы.

Пришлось сдвинуть адреса памяти, в которые пишутся режимы, яркости и тд лампы.

Точку доступа закомментировал и захардкодил свою.

Лампа добавляется в Дом как обычно - Добавить аксессуар, Другие параметры.

Лампа добавляется как цветная лампочка, включение и яркость работают сразу, а режимы надо делать через сценарии.

Чтобы сделать режим:

1 - Перебираем все цвета, пока лампа не загорится нужным режимом

![image](https://github.com/PavelLambo/GyverLampHomeKit/assets/148942618/875a2702-33a7-4715-b7bf-8454ab3270c3)
2 - Создаем сценарий, например с именем "Лампа Лава"

3 - Добавляем в сценарий включение Лампы с цветом из п1

3 - Профит
