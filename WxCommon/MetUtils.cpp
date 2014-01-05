//MetUtils.cpp
#include "stdafx.h"
#include "MetUtils.h"

float KnotsToMetresPerSec(const float Knots) { return Knots*0.51444444F; }

/*void StreamSynop(System::Windows::Forms::ImageList^ WWImages,
                 const int Width,const int Height,
                 CSynopData^ SynopData)
{
    //to save to a web server, use a memory stream - or does save allow a stream to be passed?
    System::Drawing::Bitmap^ image=gcnew Bitmap(Width,Height);
    Graphics^ g=Graphics::FromImage(image);
//    g->FillRectangle(Brushes::White,0,0,Width,Height);
    DrawSynop(g,WWImages,(float)Width/2.0f,(float)Height/2.0f,SynopData);
    //g->DrawLine(Pens::Blue,0,0,100,100);
    image->Save("c:\\synop.png",ImageFormat::Png);
}*/

//void StreamSurfaceData(System::Windows::Forms::ImageList^ WWImages,
//                 const int Width,const int Height,
//                 CWxData^ WxData)
//{
//    //to save to a web server, use a memory stream - or does save allow a stream to be passed?
//    System::Drawing::Bitmap^ image=gcnew Bitmap(Width,Height);
//    Graphics^ g=Graphics::FromImage(image);
////    g->FillRectangle(Brushes::White,0,0,Width,Height);
//    String^ Filename="c:\\synop.png";
//    //if (it is a synop) 
//    //DrawSynop(g,WWImages,(float)Width/2.0f,(float)Height/2.0f,SynopData);
//    //else if it is a metar
//    DrawMetar(g,(float)Width/2.0f,(float)Height/2.0f,(CMetarData^)WxData);
//    Filename="c:\\wximages\\"+((CMetarData^)WxData)->ICAO+".png";
//    //g->DrawLine(Pens::Blue,0,0,100,100);
//    image->Save(Filename,ImageFormat::Png);
//}

//void DrawSynop(System::Drawing::Graphics^ g,
//               System::Windows::Forms::ImageList^ WWImages,
//               const float x,const float y,
//               CSynopData^ SynopData)
//{
//    const float AirTemperatureR=32.0f; //radius
//    const float AirTemperatureA=300.0f*(float)Math::PI/180.0f; //angle in radians
//    const float DewPointTemperatureR=32.0f; //radius
//    const float DewPointTemperatureA=250.0f*(float)Math::PI/180.0f; //angle in radians
//    const float MSLPressureR=24.0f; //radius
//    const float MSLPressureA=20.0f*(float)Math::PI/180.0f; //angle in radians
//    const float PresentWeatherR=24.0f; //radius
//    const float PresentWeatherA=270.0f*(float)Math::PI/180.0f; //angle in radians
//    const float StationNameR=24.0f; //radius
//    const float StationNameA=180.0f*(float)Math::PI/180.0f; //angle in radians
//
//    System::Drawing::Font^ font = gcnew System::Drawing::Font("Arial",8);
//
////    g->DrawLine(Pens::Blue,0.0f,y,200.0f,y);
////    g->DrawLine(Pens::Blue,x,0.0f,x,200.0f);
//
//    //draw station centre, auto/manual and total cloud
//    g->DrawEllipse(Pens::Black,RectangleF(x-8,y-8,16,16));
//    DrawWindFleche(g,x,y,32,SynopData->WindDirection,(int)SynopData->WindSpeed);
//
////Air Temperature
//    float xp=AirTemperatureR*(float)Math::Sin(AirTemperatureA)+x;
//    float yp=y-AirTemperatureR*(float)Math::Cos(AirTemperatureA);
////    g->DrawLine(Pens::Blue,x,y,xp,yp);
//    if (SynopData->AirTemperature<0)
//        g->DrawString(Convert::ToString(SynopData->AirTemperature),font,Brushes::Blue,xp,yp);
//    else
//        g->DrawString(Convert::ToString(SynopData->AirTemperature),font,Brushes::Red,xp,yp);
//
////Dew Point
//    xp=DewPointTemperatureR*(float)Math::Sin(DewPointTemperatureA)+x;
//    yp=y-DewPointTemperatureR*(float)Math::Cos(DewPointTemperatureA);
////    g->DrawLine(Pens::Blue,x,y,xp,yp);
//    if (SynopData->DewPointTemperature<0)
//        g->DrawString(Convert::ToString(SynopData->DewPointTemperature),font,Brushes::Blue,xp,yp);
//    else
//        g->DrawString(Convert::ToString(SynopData->DewPointTemperature),font,Brushes::Red,xp,yp);
//
////MSL Pressure - I think this should be reduced to 3 digits
//    xp=MSLPressureR*(float)Math::Sin(MSLPressureA)+x;
//    yp=y-MSLPressureR*(float)Math::Cos(MSLPressureA);
////    g->DrawLine(Pens::Blue,x,y,xp,yp);
//    g->DrawString(Convert::ToString(SynopData->MSLPressure),font,Brushes::Red,xp,yp);
//
////Present Weather
//    xp=PresentWeatherR*(float)Math::Sin(PresentWeatherA)+x;
//    yp=y-PresentWeatherR*(float)Math::Cos(PresentWeatherA);
////    g->DrawLine(Pens::Blue,x,y,xp,yp);
//    //-8 is imageSize/2 to centre it
//    g->DrawImage(WWImages->Images[SynopData->PresentWeather],xp-8,yp-8);
//
////Station Name - need to centre this
//    xp=StationNameR*(float)Math::Sin(StationNameA)+x;
//    yp=y-StationNameR*(float)Math::Cos(StationNameA);
////    g->DrawLine(Pens::Blue,x,y,xp,yp);
//    SizeF size=g->MeasureString(SynopData->StationName,font);
//    g->DrawString(SynopData->StationName,font,Brushes::Black,xp-size.Width/2,yp);
//}

//void DrawMetar(System::Drawing::Graphics^ g,
//               const float x,const float y,
//               CMetarData^ MetarData)
//{
//    const float AirTemperatureR=32.0f; //radius
//    const float AirTemperatureA=300.0f*(float)Math::PI/180.0f; //angle in radians
//    const float DewPointTemperatureR=32.0f; //radius
//    const float DewPointTemperatureA=250.0f*(float)Math::PI/180.0f; //angle in radians
//    const float SurfacePressureR=24.0f; //radius
//    const float SurfacePressureA=20.0f*(float)Math::PI/180.0f; //angle in radians
//    const float StationNameR=24.0f; //radius
//    const float StationNameA=180.0f*(float)Math::PI/180.0f; //angle in radians
//
//    System::Drawing::Font^ font = gcnew System::Drawing::Font("Arial",8);
//
////    g->DrawLine(Pens::Blue,0.0f,y,200.0f,y);
////    g->DrawLine(Pens::Blue,x,0.0f,x,200.0f);
//
//    //draw station centre, auto/manual and total cloud
//    g->DrawEllipse(Pens::Black,RectangleF(x-8,y-8,16,16));
//    DrawWindFleche(g,x,y,32,MetarData->WindDirection,(int)MetarData->WindSpeed);
//
//    float xp,yp;
////Air Temperature
//    if (MetarData->AirTemperature!=CWxData::InvalidFloat)
//    {
//        xp=AirTemperatureR*(float)Math::Sin(AirTemperatureA)+x;
//        yp=y-AirTemperatureR*(float)Math::Cos(AirTemperatureA);
////    g->DrawLine(Pens::Blue,x,y,xp,yp);
//        if (MetarData->AirTemperature<0)
//            g->DrawString(Convert::ToString(MetarData->AirTemperature),font,Brushes::Blue,xp,yp);
//        else
//            g->DrawString(Convert::ToString(MetarData->AirTemperature),font,Brushes::Red,xp,yp);
//    }
//
////Dew Point
//    if (MetarData->DewPoint!=CWxData::InvalidFloat)
//    {
//        xp=DewPointTemperatureR*(float)Math::Sin(DewPointTemperatureA)+x;
//        yp=y-DewPointTemperatureR*(float)Math::Cos(DewPointTemperatureA);
////    g->DrawLine(Pens::Blue,x,y,xp,yp);
//        if (MetarData->DewPoint<0)
//            g->DrawString(Convert::ToString(MetarData->DewPoint),font,Brushes::Blue,xp,yp);
//        else
//            g->DrawString(Convert::ToString(MetarData->DewPoint),font,Brushes::Red,xp,yp);
//    }
//
////MSL Pressure - I think this should be reduced to 3 digits
//    if (MetarData->SurfacePressure!=CWxData::InvalidFloat)
//    {
//        xp=SurfacePressureR*(float)Math::Sin(SurfacePressureA)+x;
//        yp=y-SurfacePressureR*(float)Math::Cos(SurfacePressureA);
////    g->DrawLine(Pens::Blue,x,y,xp,yp);
//        g->DrawString(Convert::ToString(MetarData->SurfacePressure),font,Brushes::Red,xp,yp);
//    }
//
////Present Weather
////removed from metar - would need to map metar present weather codes to synop table
//
////Station Name - need to centre this
//    xp=StationNameR*(float)Math::Sin(StationNameA)+x;
//    yp=y-StationNameR*(float)Math::Cos(StationNameA);
////    g->DrawLine(Pens::Blue,x,y,xp,yp);
//    SizeF size=g->MeasureString(MetarData->StationName,font);
//    g->DrawString(MetarData->StationName,font,Brushes::Black,xp-size.Width/2,yp);
//}

//void DrawWindFleche(System::Drawing::Graphics^ g,
//                    const float x,const float y,const int length,
//                    const int direction,const int knots)
//{
//    //Circle: Calm, 0 knots
//    //Just stick: 1..2 Knots
//    //Half fleche: 3..7 Knots (intervals of 5)
//    //Full fleche: 8..12 Knots (intervals of 10)
//    //Triangle: 48..52 Knots (intervals of 50)
//
//    float const calmRadius=0.2f*length;
//    float const fsize=0.3f; //size of fleche relative to length
//    float const flecheSepLen=0.1f; //distance between this fleche and the next relative to stick length
//    float const triBase=0.15f; //size of triangle base relative to length (also affects fleche angle)
//    float const triSepLen=0.2f; //distance between this triangle and next relative to stick length
//    float const triFlecheSepLen=0.05f; //additional separation between last triangle and first fleche
//
//    
//    Pen^ blackPen=gcnew Pen(Brushes::Black);
//    blackPen->Width=2.0f;
//    //blackPen->LineJoin = System::Drawing::Drawing2D::LineJoin::MiterClipped;
//
//    if (knots==0) {
//        g->DrawEllipse(blackPen,RectangleF(x-calmRadius,y-calmRadius,calmRadius*2,calmRadius*2));
//        return;
//    }
//
//    int NumTriangles=(knots+2)/50;
//    int rem=(knots+2)%50;
//    int NumFullFleches=rem/10;
//    rem=rem%10;
//    bool HalfFleche=rem>=5;
//
//    float RadDir=(float)(direction*System::Math::PI/180.0f);
//    float xEnd=x+(float)(length*System::Math::Sin(RadDir));
//    float yEnd=y-(float)(length*System::Math::Cos(RadDir));
//    float dx=x-xEnd,dy=y-yEnd; //vector along line from end to start point
//    float nx=dy,ny=-dx; //normal to stick line
//    g->DrawLine(blackPen,x,y,xEnd,yEnd); //draw stick
//
//    float xp=xEnd,yp=yEnd;
//    PointF point1 = PointF(0.0f,0.0f);
//    PointF point2 = PointF(0.0f,0.0f);
//    PointF point3 = PointF(0.0f,0.0f);
//    array<PointF>^ triPoints = {point1,point2,point3};
//
//    //draw any triangles
//    for (int i=0; i<NumTriangles; i++) {
//        triPoints[0].X=xp;            triPoints[0].Y=yp;
//        triPoints[1].X=xp+nx*fsize;   triPoints[1].Y=yp+ny*fsize;
//        triPoints[2].X=xp+dx*triBase; triPoints[2].Y=yp+dy*triBase;
//
//        g->FillPolygon(Brushes::Black,triPoints);
//        xp+=dx*triSepLen; yp+=dy*triSepLen;
//    }
//
//    //add additional separation between last triangle and first fleche
//    if (NumTriangles>0) { xp+=dx*triFlecheSepLen; yp+=dy*triFlecheSepLen; }
//
//    //and any full fleches
//    //calculate fleche tip parallel to triangle by moving back to previous triangle tip and
//    //moving the two fleche line points along in step.
//    float xFleche=xp-dx*triBase+nx*fsize; //triBase used as fleche angle matches triangle angle
//    float yFleche=yp-dy*triBase+ny*fsize;
//    for (int i=0; i<NumFullFleches; i++) {
//        g->DrawLine(blackPen,xp,yp,xFleche,yFleche);
//        xp+=dx*flecheSepLen;      yp+=dy*flecheSepLen;
//        xFleche+=dx*flecheSepLen; yFleche+=dy*flecheSepLen;
//    }
//
//    //and a half fleche if present, taking into account that a single one is offset
//    if (HalfFleche) {
//        float xFleche=xp+(nx*fsize-dx*triBase)/2.0f; //triBase used as fleche angle matches triangle angle
//        float yFleche=yp+(ny*fsize-dy*triBase)/2.0f;
//        if (knots<8) {
//            xp+=dx*flecheSepLen;      yp+=dy*flecheSepLen;
//            xFleche+=dx*flecheSepLen; yFleche+=dy*flecheSepLen;
//        }
//        g->DrawLine(blackPen,xp,yp,xFleche,yFleche);
//    }
//
//}
