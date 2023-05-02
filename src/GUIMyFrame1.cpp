#include "GUIMyFrame1.h"

#define PANEL_WIDTH 500
#define PANEL_HEIGHT 500

float FunctionData[100][3];
int   NoPoints;

GUIMyFrame1::GUIMyFrame1( wxWindow* parent )
:
MyFrame1( parent )
{
	this->SetBackgroundColour(wxColor(192, 192, 192));
    m_bitmap.Create(PANEL_WIDTH, PANEL_HEIGHT, 24);
    m_image.Create(PANEL_WIDTH, PANEL_HEIGHT);
	PrepareData(0);
	Draw(FunctionData);
}

// ZADANIE: 
//Zadanie polega na napisaniu programu rysującego funkcje typu f(x,y). Funkcja podana jest w postaci zbioru
//wartości wi w różnych punktach (xi,yi) przy czym wiadomo, że wartości funkcji w tych punktach zawsze mieszczą się
//w zakresie od -2.5 do 2.5. Punkty są rozrzucone przypadkowo. Funkcja będzie prezentowana w postaci pseudo
//trójwymiarowego „krajobrazu” wygenerowanego metodą „voxel space” w wersji kolorowej lub w odcieniach
//szarości. Idea tej metody była wyjaśniona na wykładzie

// Zarys możliwego rozwiązania:
//Przygotowane fragmenty kodu generują GUI i nic ponad to. Do plików źródłowych dołączono również plik
//projektu programu wxFormBuilder aby można było dostosować program do swoich potrzeb. Do plików źródłowych
//dołączono również plik „PrepareData.cpp”. W odczuciu prowadzącego funkcja ta wymaga znaczącej refaktoryzacji
//(podpowiedz: czy osoba pisząca tę funkcję wiedziała, że funkcja może również zwracać std::vector? ;-P ). Oczywiście
//nie jest to konieczne.

float Shepard(int N, float d[100][3], float x, float y)
{
	float l = 0, u = 0;

	for (int i = 0; i < N; i++)
	{
		float xi = d[i][0];
		float yi = d[i][1];
		float zi = d[i][2];

		float wi = 1. / (pow(x - xi, 2) + pow(y - yi, 2));

		l += wi;
		u += wi * zi;
	}

	return u / l;
}

void GUIMyFrame1::PrepareData(int fun)
{
 int i;
 float x,y;
 srand(376257);

 switch (fun)
  {
   case 0: NoPoints=5;
           FunctionData[0][0]=-1.0;	FunctionData[0][1]= 1.0;	FunctionData[0][2]= 0.0;
	       FunctionData[1][0]= 1.0;	FunctionData[1][1]= 1.0;	FunctionData[1][2]=25.0;
	       FunctionData[2][0]= 1.0;	FunctionData[2][1]=-1.0;	FunctionData[2][2]= 5.0;
	       FunctionData[3][0]=-1.0;	FunctionData[3][1]=-1.0;	FunctionData[3][2]=25.0;
	       FunctionData[4][0]= 0.0;	FunctionData[4][1]= 0.0;	FunctionData[4][2]=15.0;
	      break;
   case 1: NoPoints=20;
           for (i=0; i<NoPoints; i++)
            {
             x=4.8*(float(rand())/RAND_MAX)-2.4;
             y=4.8*(float(rand())/RAND_MAX)-2.4;
             FunctionData[i][0]=x;
             FunctionData[i][1]=y;
             FunctionData[i][2]=200*(float(rand())/RAND_MAX);
            }
          break;
   case 2: NoPoints=100;
           for (i=0; i<NoPoints; i++)
            {
             x=4.8*(float(rand())/RAND_MAX)-2.4;
             y=4.8*(float(rand())/RAND_MAX)-2.4;
             FunctionData[i][0]=x;
             FunctionData[i][1]=y;
             FunctionData[i][2]=x*(2*x-7)*(2*y+1)+2*y;
            }
          break;
   case 3: NoPoints=10;
           for (i=0; i<NoPoints; i++)
            {
             x=4.8*(float(rand())/RAND_MAX)-2.4;
             y=4.8*(float(rand())/RAND_MAX)-2.4;
             FunctionData[i][0]=x;
             FunctionData[i][1]=y;
             FunctionData[i][2]=x*sin(2*y);
            }
          break;
   case 4: NoPoints=100;
           for (i=0; i<NoPoints; i++)
            {
             x=2*(float(rand())/RAND_MAX)-1;
             y=2*(float(rand())/RAND_MAX)-1;
             FunctionData[i][0]=x;
             FunctionData[i][1]=y;
             FunctionData[i][2]=sin(8*(x*x+y*y));
            }
          break;
    }
}

void GUIMyFrame1::Repaint()
{
    Draw(FunctionData);
}

//Przykładowa zawartość funkcji Draw(…).
//Panel ma rozmiar 500x500 i tyle wystarczy – okno nie może zmieniać rozmiarów co powinno znacznie
//ułatwić zadanie. Dla przykładu, funkcja może wyglądać następująco:
//1. Obracamy wszystkie punkty o zadany kąt rotation.
//2. Interpolujemy funkcję do regularnej siatki metodą Sheparda
//3. Wyznaczamy najmniejszą i największą wartość funkcji
//4. Rysujemy krajobraz metodą „voxel space” .


void GUIMyFrame1::Draw(float point[100][3])
{
    // Create the image with a fixed size of 500x500
    wxImage image(500, 500);

    // Rotate the points by the given angle
    float cos_t = cos(rotation);
    float sin_t = sin(rotation);

    for (int i = 0; i < NoPoints; i++) {
        float x = point[i][0];
        float y = point[i][1];

        point[i][0] = x * cos_t - y * sin_t;
        point[i][1] = x * sin_t + y * cos_t;
    }

    // Interpolate the function to a regular grid using Shepard's method
    float grid[500][500];
    float step = 2.0 / 499;

    for (int i = 0; i < 500; i++) {
        for (int j = 0; j < 500; j++) {
            float sum_w = 0.0;
            float sum_wz = 0.0;

            for (int k = 0; k < NoPoints; k++) {
                float dx = point[k][0] - (-1.0 + i * step);
                float dy = point[k][1] - (-1.0 + j * step);
                float dz = point[k][2];

                float w = 1.0 / pow(dx * dx + dy * dy, 1.5);
                sum_w += w;
                sum_wz += w * dz;
            }

            if (sum_w == 0.0) {
                grid[i][j] = 0.0;
            }
            else {
                grid[i][j] = sum_wz / sum_w;
            }
        }
    }

    // Find the minimum and maximum values of the function
    float max_z = -FLT_MAX;
    float min_z = FLT_MAX;

    for (int i = 0; i < 500; i++) {
        for (int j = 0; j < 500; j++) {
            float z = grid[i][j];
            if (z > max_z) {
                max_z = z;
            }
            if (z < min_z) {
                min_z = z;
            }
        }
    }

    // Draw the 3D landscape using the "voxel space" method
    for (int i = 0; i < 500; i++) {
        for (int j = 0; j < 500; j++) {
            float z = grid[i][j];
            unsigned char r, g, b;

            // Map the z value to a color
            if (max_z != min_z) {
                float t = (z - min_z) / (max_z - min_z);
                r = (unsigned char)((1.0 - t) * 255);
                g = 0;
                b = (unsigned char)(t * 255);
            }
            else {
                r = 0;
                g = 0;
                b = 0;
            }

            // Draw the voxel on the image
            for (int k = 0; k < 3; k++) {
                int pixel_x = i;
                int pixel_y = j;
                int pixel_z = (int)((z + 1.0) * 250);

				if (k == 0) {
					pixel_x--;
				}
				else if (k == 1) {
					pixel_y--;
				}
				else {
					pixel_z--;
				}
				
				if (pixel_x >= 0 && pixel_x < 500 &&
					pixel_y >= 0 && pixel_y < 500 &&
					pixel_z >= 0 && pixel_z < 500) {
					image.SetRGB(pixel_x, pixel_y, r, g, b);
				}
			}
		}
	}
	
	wxClientDC dc(m_panel1);
	wxBufferedDC buff_dc(&dc);
	wxBitmap bitmap(image);
	buff_dc.DrawBitmap(bitmap, 0, 0, false);
}

void GUIMyFrame1::m_button1_click( wxCommandEvent& event )
{
	// TODO: Implement m_button1_click
	PrepareData(1);
	Repaint();
}

void GUIMyFrame1::m_button2_click( wxCommandEvent& event )
{
	// TODO: Implement m_button2_click
	PrepareData(2);
	Repaint();
}

void GUIMyFrame1::m_button3_click( wxCommandEvent& event )
{
	// TODO: Implement m_button3_click
	PrepareData(3);
	Repaint();
}

void GUIMyFrame1::m_button4_click( wxCommandEvent& event )
{
	// TODO: Implement m_button4_click
	PrepareData(4);
	Repaint();
}

void GUIMyFrame1::m_cb_color_click( wxCommandEvent& event )
{
	// TODO: Implement m_cb_color_click
	ShowColor == true ? ShowColor = false : ShowColor = true;
	Repaint();
}

void GUIMyFrame1::m_s_rotation_onscroll( wxScrollEvent& event )
{
	m_st_rotation->SetLabel(wxString::Format("Obrot: %d stopni.",m_s_rotation->GetValue()));
	// TODO: Implement m_s_rotation_onscroll
	rotation = m_s_rotation->GetValue() * M_PI / 180;
	Repaint();
}

void GUIMyFrame1::m_s_tilt_onscroll( wxScrollEvent& event )
{
// TODO: Implement m_s_tilt_onscroll
	tilt = m_s_tilt->GetValue();
	Repaint();
}
