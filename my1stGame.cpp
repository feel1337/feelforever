#include <iostream>  //здесь все начинается
#include <vector>  //массив на стероидах
#include <utility>
#include <clocale>
#include <algorithm>
#include <chrono>
#include <iomanip>  //знаки после запятой в дробных числах cout << fixed << setprecision(числа после запятой) << n;
#include <cmath>  //мотемотичьку в хлам

#include <stdio.h>
#include <Windows.h>

using namespace std;

int nScreenWidth = 120;			// размер консоли X
int nScreenHeight = 40;			// размер консоли Y
int nMapWidth = 16;				// измерения мира
int nMapHeight = 16;

float fPlayerX = 14.7f;			// начальная позиция
float fPlayerY = 5.09f;
float fPlayerA = 0.0f;			// игрок начинает вращение
float fFOV = 3.14159f / 4.0f;	// fov
float fDepth = 16.0f;			// дальность видимости
float fSpeed = 5.0f;			// скорость игрока

void menu() {}  // меню

void newGame() {
    system("cls");
    cout << endl;
    cout << endl;
	cout << "\t\t\t\t\t\tLoading..." << endl;
	system("pause");
}  // новая игра
void cont() {
    system("cls");
    cout << endl;
    cout << endl;
    cout << "\t\t\t\t\t\tYou haven't saves.";
    cout << endl;
    system("pause");
    system("cls");
    cout << endl;
    cout << endl;
    cout << "\t\t\t\t\tI think You need to create new Game.";
    cout << endl;
    system("pause");
    
}  // продолжение игры
void options() {
    system("cls");
    cout << endl;
    cout << endl;
    cout << "\t\t\t\t\t\tNothing to do here." << endl;
    system("pause");
    menu();
}  // настройки

int main()
{
    setlocale(LC_ALL, "");

    menu(); {
		bool flag = false;

        do {
            system("cls"); // очистка
            cout << endl;
            cout << endl;
            cout << "\t\t\t\t\t\t1.New Game" << endl;
            cout << endl;
            cout << "\t\t\t\t\t\t2.Continue" << endl;
            cout << endl;
            cout << "\t\t\t\t\t\t3.Options" << endl;
            cout << endl;
            cout << "\t\t\t\t\t\t4.Quit" << endl;

            cout << endl;
            cout << endl;
            cout << endl;
			int ans;
            cout << "Put in number 1-4:" << endl;
            cin >> ans;

            switch (ans) {  // свобода выбора меню
            case 1: // новая игра
                newGame();
				flag = true;
                break;
            case 2: // продолжить
                cont();
                break;
            case 3: // настройки
                options();
                break;
            case 4: // выход
				system("cls");
                cout << endl;
                cout << endl;
                cout << "\t\t\t\t\t\tWe were so happy together :(" << endl;
                cout << endl;
                exit(0);
            default: // если ответ не 1-4
                cout << endl;
                cout << endl;
                cout << "\t\t\t\t\t\tWrong answer." << endl;
                system("pause");
                break;
            }
        } while (flag == false);
		
    }

	// создание буфера экрана
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	// создание карты, # - стена  . = свободное пространство
	wstring map;
	map += L"#########.......";
	map += L"#...............";
	map += L"#.......########";
	map += L"#..............#";
	map += L"#......##......#";
	map += L"#......##......#";
	map += L"#..............#";
	map += L"###............#";
	map += L"##.............#";
	map += L"#......####..###";
	map += L"#......#.......#";
	map += L"#......#.......#";
	map += L"#..............#";
	map += L"#......#########";
	map += L"#..............#";
	map += L"################";

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	while (1)
	{
		//Нам понадобится разница во времени на кадр
		//чтобы рассчитать изменение скорости движения,
		//чтобы обеспечить согласованное движение,
		//поскольку трассировка лучей не обоснована
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();


		// вращение против часовой
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000)
			fPlayerA -= (fSpeed * 0.75f) * fElapsedTime;

		// вращение по часовой
		if (GetAsyncKeyState((unsigned short)'D') & 0x8000)
			fPlayerA += (fSpeed * 0.75f) * fElapsedTime;

		// Handle Forwards movement & collision
		if (GetAsyncKeyState((unsigned short)'W') & 0x8000)
		{
			fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;;
			fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;;
				fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;;
			}
		}

		// движение назад и столкновение
		if (GetAsyncKeyState((unsigned short)'S') & 0x8000)
		{
			fPlayerX -= sinf(fPlayerA) * fSpeed * fElapsedTime;;
			fPlayerY -= cosf(fPlayerA) * fSpeed * fElapsedTime;;
			if (map.c_str()[(int)fPlayerX * nMapWidth + (int)fPlayerY] == '#')
			{
				fPlayerX += sinf(fPlayerA) * fSpeed * fElapsedTime;;
				fPlayerY += cosf(fPlayerA) * fSpeed * fElapsedTime;;
			}
		}

		for (int x = 0; x < nScreenWidth; x++)
		{
			// просчитывание угла проекции луча в пространстве
			float fRayAngle = (fPlayerA - fFOV / 2.0f) + ((float)x / (float)nScreenWidth) * fFOV;

			// дистанция до стены
			float fStepSize = 0.1f;		  // размер приращения лучей, уменьшение для увеличения разрешения
			float fDistanceToWall = 0.0f;

			bool bHitWall = false;		// true когда луч падает на стену
			bool bBoundary = false;		// true когда луч достигает границы между 2 стенами

			float fEyeX = sinf(fRayAngle); // единичный вектор луча в пространстве игрока
			float fEyeY = cosf(fRayAngle);

			// пошагово отбрасывает луч от игрока по углу луча,
			// проверяя его на пересечение с блоком
			while (!bHitWall && fDistanceToWall < fDepth)
			{
				fDistanceToWall += fStepSize;
				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerY + fEyeY * fDistanceToWall);

				// проверка, не вышел ли луч за пределы
				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight)
				{
					bHitWall = true;			// устанавливает расстояние до максимума
					fDistanceToWall = fDepth;
				}
				else
				{
					// если луч внутри,
					// проверяет не является ли яйчейка луча стенкой
					if (map.c_str()[nTestX * nMapWidth + nTestY] == '#')
					{
						// луч столкнулся со стеной
						bHitWall = true;

						// Чтобы выделить границы плитки, бросьте луч от каждого угла плитки к игроку
						//Чем больше совпадает этот луч с лучом рендеринга,
						//тем ближе мы к границе плитки, которую мы заштрихуем,
						//чтобы добавить детали к стенам
						vector<pair<float, float>> p;

						// проверка каждого угла удара плитки
						for (int tx = 0; tx < 2; tx++)
							for (int ty = 0; ty < 2; ty++)
							{
								// градусы от угла к глазу
								float vy = (float)nTestY + ty - fPlayerY;
								float vx = (float)nTestX + tx - fPlayerX;
								float d = sqrt(vx * vx + vy * vy);
								float dot = (fEyeX * vx / d) + (fEyeY * vy / d);
								p.push_back(make_pair(d, dot));
							}

						// сортировать пары от ближайшего к дальнему
						sort(p.begin(), p.end(), [](const pair<float, float>& left, const pair<float, float>& right) {return left.first < right.first; });

						// первые 2/3 самые близкие (4 мы никогда не увидим)
						float fBound = 0.01;
						if (acos(p.at(0).second) < fBound) bBoundary = true;
						if (acos(p.at(1).second) < fBound) bBoundary = true;
						if (acos(p.at(2).second) < fBound) bBoundary = true;
					}
				}
			}

			// расчитывание расстояния до потолка и пола
			int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
			int nFloor = nScreenHeight - nCeiling;

			// текстуры стен зависят от дистанции
			short nShade = ' ';
			if (fDistanceToWall <= fDepth / 4.0f)			nShade = 0x2588;	// ближе всего
			else if (fDistanceToWall < fDepth / 2.0f)		nShade = 0x2592;
			else if (fDistanceToWall < fDepth)				nShade = 0x2591;
			else											nShade = ' ';		// вне зоны видимости

			if (bBoundary)		nShade = ' '; // делит стены (можно убрать)

			for (int y = 0; y < nScreenHeight; y++)
			{
				// каждый ряд
				if (y <= nCeiling)
					screen[y * nScreenWidth + x] = ' ';
				else if (y > nCeiling && y <= nFloor)
					screen[y * nScreenWidth + x] = nShade;
				else // пол
				{
					// текстура пола зависит от дистанции
					float b = 1.0f - (((float)y - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));
					if (b < 0.25)		nShade = '#';
					else if (b < 0.5)	nShade = 'x';
					else if (b < 0.75)	nShade = '.';
					else if (b < 0.9)	nShade = '-';
					else				nShade = ' ';
					screen[y * nScreenWidth + x] = nShade;
				}
			}
		}

		// отображение позиции и фпс
		swprintf_s(screen, 40, L"X=%3.2f, Y=%3.2f, A=%3.2f FPS=%3.2f ", fPlayerX, fPlayerY, fPlayerA, 1.0f / fElapsedTime);

		// отображение карты
		for (int nx = 0; nx < nMapWidth; nx++)
			for (int ny = 0; ny < nMapWidth; ny++)
			{
				screen[(ny + 1) * nScreenWidth + nx] = map[ny * nMapWidth + nx];
			}
		screen[((int)fPlayerX + 1) * nScreenWidth + (int)fPlayerY] = 'P';

		// рамка экрана
		screen[nScreenWidth * nScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(hConsole, screen, nScreenWidth * nScreenHeight, { 0,0 }, &dwBytesWritten);
	}

    return 0;
}