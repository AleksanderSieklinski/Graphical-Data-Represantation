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
	Draw(NoPoints, FunctionData, ShowColor, alpha);
	Repaint();
}

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
    wxMemoryDC memDC;
	memDC.SelectObject(m_bitmap);
	wxClientDC dc(m_panel1);
	dc.Blit(0, 0, 500, 500, &memDC, 0, 0);
	memDC.SelectObject(wxNullBitmap);
}

void GUIMyFrame1::Draw(int N, float d[100][3],bool ShowColor, double alpha)
{
	wxMemoryDC memDC;
	memDC.SelectObject(m_bitmap);
	memDC.SetBackground(*wxWHITE_BRUSH);
	memDC.Clear();
	memDC.SetPen(*wxBLACK_PEN);
	memDC.SetBrush(*wxTRANSPARENT_BRUSH);

	float width = 1;
	float height = 3 + tilt * 0.07;

	PrepareData(choice);

	float coso = cos(alpha);
	float sino = sin(alpha);

	for (int i = 0; i < N; i++) {
		float x = d[i][0];
		float y = d[i][1];

		d[i][0] = x * coso - y * sino;
		d[i][1] = x * sino + y * coso;
	}


	auto values = new float[400][400];
	for (int y = 0; y < 400; y++)
		for (int x = 0; x < 400; x++)
		{
			values[x][y] = Shepard(N, d, x*1.25 / 100.0 - 2.5, -y * 1.25 / 100.0 + 2.5);
		}
	float min = d[0][2], max = d[0][2];
	for (int i = 0; i < N; i++) {
		if (d[i][2] < min)
			min = d[i][2];
		if (d[i][2] > max)
			max = d[i][2];
	}

	if (ShowColor == true) {
		unsigned char* data = new unsigned char[480000];
		for (int x = 0; x < 400; x++) {
			for (int y = 0; y < 400; y++) {
				float color = ((values[x][y]) - min) / (max - min);
				data[y * 400 * 3 + x * 3 + 0] = 255 * color;
				data[y * 400 * 3 + x * 3 + 1] = 0;
				data[y * 400 * 3 + x * 3 + 2] = 255 - 255 * color;
			}
		}

		for (int i = 0; i < 40; i++) {
			for (int j = 0; j < 80; j++) {
				float color = ((values[i * 10][j * 5] - min)) / (max - min) * 50 + 51;
				memDC.SetPen(wxColor(data[j * 5 * 400 * 3 + i * 10 * 3 + 0], data[j * 5 * 400 * 3 + i * 10 * 3 + 1], data[j * 5 * 400 * 3 + i * 10 * 3 + 2]));

				memDC.SetBrush(wxColor(data[j * 5 * 400 * 3 + i * 10 * 3 + 0], data[j * 5 * 400 * 3 + i * 10 * 3 + 1], data[j * 5 * 400 * 3 + i * 10 * 3 + 2]));
				memDC.DrawRectangle(500 - (width*i + width * j * 5), 50 + height * i - color, width * 10, color);
			}
		}
		return;

	}

	if (ShowColor == false) {
		unsigned char* data = new unsigned char[480000];
		unsigned char* new_data = new unsigned char[480000];
		for (int x = 0; x < 400; x++) {
			for (int y = 0; y < 400; y++) {
				float color = (values[x][y] - min) / (max - min);
				data[y * 400 * 3 + x * 3 + 0] = 255 * color;
				data[y * 400 * 3 + x * 3 + 1] = 255 * color;
				data[y * 400 * 3 + x * 3 + 2] = 255 * color;
			}

		}

		for (int i = 0; i < 40; i++) {
			for (int j = 0; j < 80; j++) {
				float color = ((values[i * 10][j * 5] - min)) / (max - min) * 50 + 51;
				memDC.SetPen(wxColor(data[j * 5 * 400 * 3 + i * 10 * 3 + 0], data[j * 5 * 400 * 3 + i * 10 * 3 + 1], data[j * 5 * 400 * 3 + i * 10 * 3 + 2]));
				memDC.SetBrush(wxColor(data[j * 5 * 400 * 3 + i * 10 * 3 + 0], data[j * 5 * 400 * 3 + i * 10 * 3 + 1], data[j * 5 * 400 * 3 + i * 10 * 3 + 2]));
				memDC.DrawRectangle(500 - (width*i + width * j * 5), 50 + height * i - color, width * 10, color);
			}
		}

	}

}

void GUIMyFrame1::m_button1_click( wxCommandEvent& event )
{

	// TODO: Implement m_button1_click
	PrepareData(1);
	choice = 1;
    Draw(NoPoints, FunctionData, ShowColor, alpha);
	Repaint();
}

void GUIMyFrame1::m_button2_click( wxCommandEvent& event )
{
	// TODO: Implement m_button2_click
	PrepareData(2);
	choice = 2;
    Draw(NoPoints, FunctionData, ShowColor, alpha);
	Repaint();
}

void GUIMyFrame1::m_button3_click( wxCommandEvent& event )
{
	// TODO: Implement m_button3_click
	PrepareData(3);
	choice = 3;
    Draw(NoPoints, FunctionData, ShowColor, alpha);
	Repaint();
}

void GUIMyFrame1::m_button4_click( wxCommandEvent& event )
{
	// TODO: Implement m_button4_click
	PrepareData(4);
	choice = 4;
    Draw(NoPoints, FunctionData, ShowColor, alpha);
	Repaint();
}

void GUIMyFrame1::m_cb_color_click( wxCommandEvent& event )
{
	// TODO: Implement m_cb_color_click
	ShowColor == true ? ShowColor = false : ShowColor = true;
	Draw(NoPoints, FunctionData, ShowColor, alpha);
	Repaint();
}

void GUIMyFrame1::m_s_rotation_onscroll( wxScrollEvent& event )
{
	m_st_rotation->SetLabel(wxString::Format("Obrot: %d stopni.",m_s_rotation->GetValue()));
	// TODO: Implement m_s_rotation_onscroll
	alpha = m_s_rotation->GetValue() * M_PI / 180;
	Draw(NoPoints, FunctionData, ShowColor, alpha);
	Repaint();
}

void GUIMyFrame1::m_s_tilt_onscroll( wxScrollEvent& event )
{
// TODO: Implement m_s_tilt_onscroll
	tilt = m_s_tilt->GetValue();
    Draw(NoPoints, FunctionData, ShowColor, alpha);
	Repaint();
}
