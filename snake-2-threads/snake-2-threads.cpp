#include <iostream>
#include <windows.h>
#include <conio.h>
#include <thread>
#include <queue>
#include <atomic>
using namespace std;



struct SpinlockMutex
{
	atomic_flag flag = ATOMIC_FLAG_INIT;
	void lock()
	{
		while (flag.test_and_set(std::memory_order_acquire));
	}
	void unlock()
	{
		flag.clear(std::memory_order_release);
	}
};
SpinlockMutex myMytex;


queue<int> dataQueue; //для передачи нажатий клавиш
queue<bool> winQueue; //для определения win/loss

int main()
{
	winQueue.push(false);
	srand(time(0)); // запуск генератора случайных чисел
	system("title Snake Game");
	system("mode con cols=70 lines=31"); // установка размеров окна консоли
	MoveWindow(GetConsoleWindow(), 50, 50, 2000, 2000, true); // установка стартовой позиции окна консоли (50 и 50 - это пиксели
	// относительно верхнего левого угла монитора
	const int width = 50, height = 30; // размеры поля, по которому бегает змейка

	const int max_length = 6; // установка максимальной длины "змейки"
	const int max_length2 = 6;
	int array_X[max_length]; // массив,хранящий абсциссы звеньев "змейки"
	int array_Y[max_length];// массив, хранящий ординаты звеньев "змейки"
	int array_X2[max_length];
	int array_Y2[max_length];

	int length = 1; // переменная длины "змейки"
	int length2 = 1;
	array_X[0] = width / 2; // установка стартовой абсциссы "змейки"
	array_Y[0] = height / 2; // установка стартовой ординаты "змейки"
	array_X2[0] = (width / 2) + 1; 
	array_Y2[0] = (height / 2) + 1;

	int dx = 1, dy = 0; // создание смещений по осям для движения "змейки"
	int dx2 = 1, dy2 = 0;
	int X_apple; // абсцисса "яблока"
	int Y_apple; // ордината "яблока"
	
	int sleep_time = 300; // переменная частоты кадров SPEED

	char snake = '*'; // символ для отображения тела "змейки"
	char snake2 = '*';
	char apple = 'o'; // символ для отображения "яблока"
	char head = 1; // символ для отображения головы "змейки"
	char head2 = 1;
	COORD c; // объект для хранения координат
	COORD apple_coordinate;
	COORD c2;
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE); // создание хендла потока вывода
	HANDLE h2 = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cci = { sizeof(cci), false }; // создание параметров на отображение курсора
	CONSOLE_CURSOR_INFO cci2 = { sizeof(cci2), false };
	SetConsoleCursorInfo(h, &cci); //связывание параметров и хендла
	SetConsoleCursorInfo(h2, &cci2);

	SetConsoleTextAttribute(h, 4); // установка цвета, которым рисуется рамка поля
	for (int y = 0; y < height; y++) //  отрисовка рамки
	{
		for (int x = 0; x < width; x++)
		{
			char s;
			if (x == 0 && y == 0) // в верхнем левом углу поля - символ соответствующего угла
				s = '#'; // 218
			else if (x == 0 && y == height - 1) // нижний левый угол
				s = '#'; // 192
			else if (y == 0 && x == width - 1) // верхний правый угол
				s = '#'; // 191
			else if (y == height - 1 && x == width - 1) // нижний правый угол
				s = '#'; // 217
			else if (y == 0 || y == height - 1) // верхняя и нижняя граница поля
				s = '#'; // 196
			else if (x == 0 || x == width - 1) // левая и правая граница поля
				s = '#'; // 179
			else s = ' '; // во всех остальных случаях должен быть просто пробел (означает пустую область поля)
			cout << s; // выводим символ

		}
		cout << "\n";
	}

	do // цикл ставит координаты яблока случанйм образом - но чтобы они не совпадали со "змейкой"
	{

		X_apple = rand() % (width - 2) + 1;
		Y_apple = rand() % (height - 2) + 1;
	} while (X_apple != array_X[length - 1] && Y_apple != array_Y[length - 1] && X_apple != array_X2[length2 - 1] && Y_apple != array_Y2[length2 - 1]);

	apple_coordinate.X = X_apple; // связываем объект координат с позициями "яблока"
	apple_coordinate.Y = Y_apple;
	SetConsoleCursorPosition(h, apple_coordinate); // отправляем курсор на позицию "яблока"
	SetConsoleTextAttribute(h, 12); // устанавливаем красный цвет для отрисовки "яблока"
	cout << apple; // отображаем символ "яблока"

	c.X = array_X[0]; // связываем объект координат со стартовой позицией "змейки"
	c.Y = array_Y[0];
	SetConsoleCursorPosition(h, c); // отправляем курсор на позицию головы "змейки"
	SetConsoleTextAttribute(h, 10); // устанавливаем зеленый цвет для отрисовки "змейки"
	cout << head; // отображаем символ головы "змейки"

	c2.X = array_X2[0];
	c2.Y = array_Y2[0];
	SetConsoleCursorPosition(h2, c2);
	SetConsoleTextAttribute(h2, 11);
	cout << head2;

	bool flag = true;
		
	thread play2([&]()
		{

			this_thread::sleep_for(chrono::milliseconds(50));
			do
			{
				Sleep(sleep_time);
				int k = 0;
				if (!dataQueue.empty())
				{
					 k = dataQueue.front(); // считываем код клавиши из буфера
						dataQueue.pop();
				}
					switch (k) // пропускаем код нажатой клавиши внутрь оператора выбора
					{
					
					case  115:
						dy2 = 1;
						dx2 = 0;
						break;
					
					case 119: // если вверх
						dy2 = -1; // аналогично согласно геометрической логике
						dx2 = 0;
						break;

					case 97: // если влево
						dy2 = 0;
						dx2 = -1;
						break;
					
					case 100: // если вправо
						dy2 = 0;
						dx2 = 1;
						break;

					case 27: // если была нажата клавиша ESC
						flag = false; // устанавливаем флажок на ЛОЖЬ, чтоб закончить показ движения
						break;
					}
				
				 // определяем значение абсциссы головы "змейки" после смещения
				 // то же самое для ординаты
				int X2 = array_X2[length2 - 1] + dx2;
				int Y2 = array_Y2[length2 - 1] + dy2;

				auto rezalt_game = winQueue.front();
				if (rezalt_game)
				{
					break;
				}

				if ( X2 == X_apple && Y2 == Y_apple) // проверка на достижение "яблока"
				{

					if (X2 == X_apple && Y2 == Y_apple)
					{
						c2.X = array_X2[length2 - 1];
						c2.Y = array_Y2[length2 - 1];
						SetConsoleCursorPosition(h2, c2);
						SetConsoleTextAttribute(h2, 11);
						cout << snake2;

						length2++;
						c2.X = array_X2[length2 - 1] = X2;
						c2.Y = array_Y2[length2 - 1] = Y2;
						SetConsoleCursorPosition(h2, c2);
						SetConsoleTextAttribute(h2, 11);
						cout << head2;

					}
			
					if (length2 == max_length2) // проверка, достигла ли длина "змейки" своего максимального значения
					{
						
						cout << " Win Player 2 BLUE ";
						winQueue.pop();
						winQueue.push(true);
						this_thread::sleep_for(chrono::milliseconds(2000));
						SetConsoleTextAttribute(h2, 11);
						break;

					}

					int i, r; // переменная для подсчета количества звеньев "змейки", не совпадающих с позицией "яблока"
					do {
						X_apple = rand() % (width - 2) + 1; // установка новых координат "яблока"
						Y_apple = rand() % (height - 2) + 1;
						i = 0; r = 0;// обнуление числа несовпадающих координат
						for (; i < length; i++) // запуск цикла на сверку совпадений
							if (X_apple == array_X[i] && Y_apple == array_Y[i]) // если совпадение найдено
								break;

						for (; r < length2; r++)
							if (X_apple == array_X2[r] && Y_apple == array_Y2[r])
								break;

					} while (i < length || r < length2); // поиск новых координат продолжается, пока число несовпадающих координат меньше длины "змейки"

					apple_coordinate.X = X_apple; // установка в объект координат новой корректной позиции "яблока"
					apple_coordinate.Y = Y_apple;
					SetConsoleCursorPosition(h, apple_coordinate); // отправка туда курсора
					SetConsoleTextAttribute(h, 12); // установка цвета в красный
					cout << apple; // отображение символа "яблока"
				}

				else // случай, когда голова "змейки" оказалась на новой пустой позиции
				{

					// переменная на количество звеньев, не совпадающих с новой позицией - кроме хвоста "змейки"
					int r = 1;
				
					for (; r < length2; r++)
						if (X2 == array_X2[r] && Y2 == array_Y2[r]) // если совпадение найдено в цикле - прерываемся
						{
							//SetConsoleTextAttribute(h, 10);
							cout << " Crash - Win Player 1 GREEN ";
							winQueue.pop();
							winQueue.push(true);
							SetConsoleTextAttribute(h, 10);

							this_thread::sleep_for(chrono::milliseconds(2000));
							break;
						};

					if (r < length2) // если число несовпадающих звеньев меньше длины "змейки" - то прерываем основной цикл игры
						break;

					else  // а иначе запускаем обработку сдвига "змейки"
					{

						if (X2 != 0 && X2 != width - 1 && Y2 != 0 && Y2 != height - 1)
						{
							c2.X = array_X2[0];
							c2.Y = array_Y2[0];
							SetConsoleCursorPosition(h2, c2);
							cout << ' ';
						}
					
						if (X2 != 0 && X2 != width - 1 && Y2 != 0 && Y2 != height - 1)
						{
							if (length2 > 1)
							{
								c2.X = array_X2[length2 - 1];
								c2.Y = array_Y2[length2 - 1];
								SetConsoleCursorPosition(h2, c2);
								SetConsoleTextAttribute(h2, 11);
								putchar(snake2);
							}
						}

						if (X2 != 0 && X2 != width - 1 && Y2 != 0 && Y2 != height - 1)
						{
							for (int r = 0; r < length2 - 1; r++)
							{
								array_X2[r] = array_X2[r + 1];
								array_Y2[r] = array_Y2[r + 1];

					}
					}

						if (X2 != 0 && X2 != width - 1 && Y2 != 0 && Y2 != height - 1)
						{
							c2.X = array_X2[length2 - 1] = X2;
							c2.Y = array_Y2[length2 - 1] = Y2;
							SetConsoleCursorPosition(h2, c2);
							SetConsoleTextAttribute(h2, 11);
							cout << head2;

						}

						if (length2 <= 0) { continue; }
						else
						{
							for (size_t i = 0; i < length2; i++) //отрисовка змейки после пересечения
							{
								
								c2.X = array_X2[i];
								c2.Y = array_Y2[i];
								SetConsoleCursorPosition(h2, c2);
								SetConsoleTextAttribute(h2, 11);
								if (i == length2 - 1) {
									cout << head2;
									break;
								}
								putchar(snake2);
							}
						}

				
					}

				}

			}while (flag);
			this_thread::sleep_for(chrono::milliseconds(2000));
			system("cls");
		}
	);
		//bool flag = true; // переменная для управления ходом цикла
	do //  цикл игры
	{
		Sleep(sleep_time); // задержка потока программы на заданный ранее интервал

		if (_kbhit()) // проверяем, была ли нажата какая-либо клавиша и запускаем её обработку в случае ИСТИНЫ
		{
			int k = _getch(); // считываем код клавиши из буфера
			if (k == 0 || k == 224) // если первый код - вспомогательный, считываем второй код
				k = _getch();
			if (k == 115 || k == 119 || k == 97 || k == 100 || k == 27)
			{
				dataQueue.push(k);
			}
				switch (k) // пропускаем код нажатой клавиши внутрь оператора выбора
				{
				case 80: // если была нажата клавиша вниз
					dy = 1; // то приращение по оси ординат будет положительным
						dx = 0;// по оси абсцисс приращение нулевое
						break;

				case 72: // если вверх
					dy = -1; // аналогично согласно геометрической логике
					dx = 0;
					break;
			
				case 75: // если влево
					dy = 0;
					dx = -1;
					break;
				
				case 77: // если вправо
					dy = 0;
					dx = 1;
					break;

				case 27: // если была нажата клавиша ESC
					flag = false; // устанавливаем флажок на ЛОЖЬ, чтоб закончить показ движения
					break;
				}
		}

		int X = array_X[length - 1] + dx; // определяем значение абсциссы головы "змейки" после смещения
		int Y = array_Y[length - 1] + dy; // то же самое для ординаты
	
		if (X == X_apple && Y == Y_apple ) // проверка на достижение "яблока"
		{

			if (X == X_apple && Y == Y_apple)
			{
				c.X = array_X[length - 1]; // установка в объект координат позиции головы "змейки"
				c.Y = array_Y[length - 1];
				SetConsoleCursorPosition(h, c); // установка курсора в эту позицию
				SetConsoleTextAttribute(h, 10);
				cout << snake; // отображение символа тела "змейки"

				length++; // увеличение длины "змейки" (яблоко проглочено)
				c.X = array_X[length - 1] = X; // установка в массивы позиции нового звена "змейки"
				c.Y = array_Y[length - 1] = Y;

				SetConsoleCursorPosition(h, c); // установка туда курсора
				SetConsoleTextAttribute(h, 10);
				cout << head; // и отображение там символа головы "змейки"
			}

			if (length == max_length) // проверка, достигла ли длина "змейки" своего максимального значения
			{
			 cout << " Win Player 1 GREEN ";
			 winQueue.pop();
			 winQueue.push(true);
				this_thread::sleep_for(chrono::milliseconds(2000));
				break;
			}

			int i, r; // переменная для подсчета количества звеньев "змейки", не совпадающих с позицией "яблока"
			do {
				X_apple = rand() % (width - 2) + 1; // установка новых координат "яблока"
				Y_apple = rand() % (height - 2) + 1;
				i = 0; r = 0;// обнуление числа несовпадающих координат
				for (; i < length; i++) // запуск цикла на сверку совпадений
					if (X_apple == array_X[i] && Y_apple == array_Y[i]) // если совпадение найдено
						break;

				for (; r < length2; r++)
					if (X_apple == array_X2[r] && Y_apple == array_Y2[r])
						break;

			} while (i < length || r < length2); // поиск новых координат продолжается, пока число несовпадающих координат меньше длины "змейки"

			apple_coordinate.X = X_apple; // установка в объект координат новой корректной позиции "яблока"
			apple_coordinate.Y = Y_apple;
			SetConsoleCursorPosition(h, apple_coordinate); // отправка туда курсора
			SetConsoleTextAttribute(h, 12); // установка цвета в красный
			cout << apple; // отображение символа "яблока"
	
		}

		else // случай, когда голова "змейки" оказалась на новой пустой позиции
		{

			int i = 1;// переменная на количество звеньев, не совпадающих с новой позицией - кроме хвоста "змейки"
			
			for (; i < length; i++)
				if (X == array_X[i] && Y == array_Y[i]) // если совпадение найдено в цикле - прерываемся
				{
					//SetConsoleTextAttribute(h, 11);
					cout << " Crash - Win Player 2 BLUE ";
					winQueue.pop();
					winQueue.push(true);
					SetConsoleTextAttribute(h, 11);
					this_thread::sleep_for(chrono::milliseconds(2000));
					break;
				};

			if (i < length) // если число несовпадающих звеньев меньше длины "змейки" - то прерываем основной цикл игры
				break;

			else  // а иначе запускаем обработку сдвига "змейки"
			{

				if (X != 0 && X != width - 1 && Y != 0 && Y != height - 1)
				{
					c.X = array_X[0]; // устанавливаем в объект координат позицию хвоста "змейки"
					c.Y = array_Y[0];
					SetConsoleCursorPosition(h, c); // двигаем туда курсор
					cout << ' '; // и отображаем пробел (затирка хвоста)
				}

				if (X != 0 && X != width - 1 && Y != 0 && Y != height - 1)
				{
					if (length > 1) // если длина змейки больше 
					{
						c.X = array_X[length - 1]; // устанавливаем в объект координат предыдущую позицию головы "змейки"
						c.Y = array_Y[length - 1];
						SetConsoleCursorPosition(h, c);  // двигаем туда курсор
						SetConsoleTextAttribute(h, 10);
						putchar(snake); // выводим символ тела "змейки"
					}
				}

				if (X != 0 && X != width - 1 && Y != 0 && Y != height - 1)
				{
					for (int i = 0; i < length - 1; i++) // запускаем цикл свдига координат звеньев "змейки"
					{
						array_X[i] = array_X[i + 1]; // обрабатываем все звенья - кроме последнего
						array_Y[i] = array_Y[i + 1];

					}
				}

				if (X != 0 && X != width - 1 && Y != 0 && Y != height - 1)
				{
					c.X = array_X[length - 1] = X; // устанавливаем новую позицию головы "змейки"
					c.Y = array_Y[length - 1] = Y;
					SetConsoleCursorPosition(h, c); // двигаем туда курсора
					SetConsoleTextAttribute(h, 10);
					cout << head; // отображаем символ головы "змейки"
				}

				if (length <= 0) { continue; }
				else
				{
					for (size_t i = 0; i < length; i++)
					{

						c.X = array_X[i];
						c.Y = array_Y[i];
						SetConsoleCursorPosition(h, c);
						SetConsoleTextAttribute(h, 10);
						if (i == length - 1) {
							cout << head;
							break;
						}
						putchar(snake);
					}
				}

			}

		}
		auto rezal_game = winQueue.front();
		if (rezal_game)
		{
			break;
		}

	} while (flag); // выходим из цикла, если сброшена управляющая переменная
	
	system("cls"); // очищаем экран
	cout << "GAME OVER\n"; // выводим сообщение о конце игры

};
