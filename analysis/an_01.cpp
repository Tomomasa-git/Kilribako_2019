#include<iostream>  
#include<cmath>     //数学関数いろいろ
#include<string>    //読み書きに便利らしい
#include<fstream>   //ファイル入出力

using namespace std;

int main(void){
    double len0, h0, len1, h1, t0, t1, cix0_1, ciz0_1, cfx0_1, cfz0_1, cix0_2, ciz0_2, cfx0_2, cfz0_2, cix1_1, ciz1_1, cfx1_1, cfz1_1, cix1_2, ciz1_2, cfx1_2, cfz1_2, cx0_1, cz0_1, cx0_2, cz0_2, cx1_1, cz1_1, cx1_2, cz1_2, ct0, ct1, clx0, clz0, clx1, clz1;
    double tag[50], ix1[50], iz1[50], fx1[50], fz1[50], ix2[50], iz2[50], fx2[50], fz2[50];
    double X0[50], Z0[50], X1[50], Z1[50], LX[50], LY[50], LZ[50], R[50], E[50], Min_R = 0.0, Min_E = 0.0, Max_R = 0.0, Max_E = 0.0, Sum_R = 0.0, Sum_E = 0.0;
    double d = 0.4, H_R[50], H_E[50];
    int i = 0, j = 0, k = 0,  max;
    FILE *in_c, *in_d, *out_r1, *out_r2;
    //念のため配列初期化
    for ( i = 0; i < 50; i++)
    {
        tag[i] = 0.0;
        ix1[i] = 0.0;
        iz1[i] = 0.0;
        fx1[i] = 0.0;
        fz1[i] = 0.0;
        ix2[i] = 0.0;
        iz2[i] = 0.0;
        fx2[i] = 0.0;
        fz2[i] = 0.0;
        
        X0[i] = 0.0;
        Z0[i] = 0.0;
        X1[i] = 0.0;
        Z1[i] = 0.0;

        LX[i] = 0.0;
        LY[i] = 0.0;
        LZ[i] = 0.0;

        R[i] = 0.0;
        E[i] = 0.0;

        H_R[i] = 0.0;
        H_E[i] = 0.0;
    }
    


    //飛程読み取りデータ読み込み
    in_d = fopen("0711_01.dat", "r");

    for ( i = 0; i < 50; i++)
    {
        j ++;
        if(EOF == fscanf(in_d, "%lf%lf%lf%lf%lf%lf%lf%lf%lf", &tag[i], &ix1[i], &iz1[i], &fx1[i], &fz1[i], &ix2[i], &iz2[i], &fx2[i], &fz2[i] )) break;/* code */
    }
    max = j;

    cout << endl;
    cout << max << " ";
    cout << endl;

    fclose(in_d);

    cout << endl;
    for ( i = 0; i < max - 1; i++)
    {
        cout << tag[i] << " " << ix1[i] << " " << iz1[i] << " " << fx1[i] << " " << fz1[i] << " " << ix2[i] << " " << iz2[i] << " " << fx2[i] << " " << fz2[i] ;
        cout << endl;
    }    
    cout << endl;



    //角度算出
    ////0:40side, 1:30side
    len0 = 48.3;
    h0 = 101.5;
    len1 = 28.3;
    h1 = 100.5;

    t0 = atan(h0/len0);
    t1 = atan(h1/len1);


    //参照画像から長さ較正用数値算出
    ////0:40side, 1:30side
    cix0_1 = 264.0;    //始点の値
    ciz0_1 = 195.0;
    cfx0_1 = 302.0;    //終点の値
    cfz0_1 = 198.0;
    cix0_2 = 264.0;    //始点の値
    ciz0_2 = 195.0;
    cfx0_2 = 267.0;    //終点の値
    cfz0_2 = 159.0;

    cix1_1 = 308.0;    //始点の値
    ciz1_1 = 485.0;
    cfx1_1 = 348.0;    //終点の値
    cfz1_1 = 492.0;
    cix1_2 = 308.0;    //始点の値
    ciz1_2 = 485.0;
    cfx1_2 = 315.0;    //終点の値
    cfz1_2 = 447.0;

    ////0_40side
    cx0_1 = cfx0_1 - cix0_1;
    cz0_1 = ciz0_1 - cfz0_1;
    cx0_2 = cfx0_2 - cix0_2;
    cz0_2 = ciz0_2 - cfz0_2;

    //////釣り糸の方向に合わせるように角度を求め回転変換
    ct0 = atan(cz0_1/cx0_1);

    cx0_1 = cx0_1*cos(ct0) + cz0_1*sin(ct0);
    cz0_1 = -1*cx0_1*sin(ct0) + cz0_1*cos(ct0);
    cx0_2 = cx0_2*cos(ct0) + cz0_2*sin(ct0);
    cz0_2 = -1*cx0_2*sin(ct0) + cz0_2*cos(ct0);

    //////1cm当たりのピクセル数を算出
    clx0 = sqrt(pow(cx0_1, 2.0) + pow(cz0_1, 2.0)); 
    clz0 = sqrt(pow(cx0_2, 2.0) + pow(cz0_2, 2.0)); 

    //1_30side
    cx1_1 = cfx1_1 - cix1_1;
    cz1_1 = ciz1_1 - cfz1_1;
    cx1_2 = cfx1_2 - cix1_2;
    cz1_2 = ciz1_2 - cfz1_2;

    //////釣り糸の方向に合わせるように角度を求め回転変換
    ct1 = atan(cz1_1/cx1_1);

    cx1_1 = cx1_1*cos(ct1) + cz1_1*sin(ct1);
    cz1_1 = -1*cx1_1*sin(ct1) + cz1_1*cos(ct1);
    cx1_2 = cx1_2*cos(ct1) + cz1_2*sin(ct1);
    cz1_2 = -1*cx1_2*sin(ct1) + cz1_2*cos(ct1);

    //////1cm当たりのピクセル数を算出
    clx1 = sqrt(pow(cx1_1, 2.0) + pow(cz1_1, 2.0)); 
    clz1 = sqrt(pow(cx1_2, 2.0) + pow(cz1_2, 2.0)); 



    //読み込んだデータから飛程、エネルギー算出  ////0:40side, 1:30side
    cout << "-----output calculated range and energy-----" ;
    cout << endl;
    cout << "#data number_" << "_Range[cm]_" << "_Energy[MeV]";
    cout << endl;

    for ( i = 0; i < max - 1; i++)
    {
        //40side
        X0[i] = ((fx2[i] - ix2[i])*cos(ct0) + (iz2[i] - fz2[i])*sin(ct0))/clx0;
        Z0[i] = (-1.0*(fx2[i] - ix2[i])*sin(ct0) + (iz2[i] - fz2[i])*cos(ct0))/clz0;//cm単位で長さを算出

        //30side
        X1[i] = ((fx1[i] - ix1[i])*cos(ct1) + (iz1[i] - fz1[i])*sin(ct1))/clx1;
        Z1[i] = (-1.0*(fx1[i] - ix1[i])*sin(ct1) + (iz1[i] - fz1[i])*cos(ct1))/clz1;//cm単位で長さを算出

        //両者のデータを統合し、各方向の成分として計算
        LX[i] = 0.5*(X0[i] + X1[i]);
        LY[i] = (Z0[i]*cos(t1) - Z1[i]*cos(t0))/sin(t0 + t1);
        LZ[i] = (Z0[i]*sin(t1) + Z1[i]*sin(t0))/sin(t0 + t1);

        //統合したベクトル成分から飛程算出
        R[i] = sqrt(pow(LX[i], 2.0) + pow(LY[i], 2.0) + pow(LZ[i], 2.0));

        //飛程[cm]から運動エネルギー[MeV]に換算
        E[i] = pow(0.318, -2.0/3.0)*pow(R[i], 2.0/3.0);

        cout << tag[i] << " " << R[i] << " " << E[i] << " ";
        cout << endl;
    }
    
    cout << endl;

    Min_R = R[0];
    Max_R = R[0];
    Min_E = E[0];
    Max_E = E[0];    
    for ( i = 0; i < max - 1; i++)
    {
        Sum_R = Sum_R + R[i];
        Sum_E = Sum_E + E[i];

        if (Min_R > R[i]){
            Min_R = R[i];/* code */
        } else;
        if (Max_R < R[i]){
            Max_R = R[i];/* code */
        } else;
        
        if (Min_E > E[i]){
            Min_E = E[i];/* code */
        } else;
        if (Max_E < E[i]){
            Max_E = E[i];/* code */
        } else;        /* code */     
    }
    cout << endl;
    cout << "#Range   Ave:" << Sum_R/(max - 1.) << " " << "Min:" << Min_R << " " << "Max:" << Max_R << "" << endl;
    cout << "#Energy   Ave:" << Sum_E/(max - 1.) << " " << "Min:" << Min_E << " " << "Max:" << Max_E << "" << endl;
    cout << endl;

    //計算結果をファイルに出力
    ofstream writing_file_01;
    writing_file_01.open("0711_result01.dat", ios::out);
    writing_file_01 << "#-----output calculated range and energy-----" << endl;
    writing_file_01 << "#Range   Ave:" << Sum_R/(max - 1.) << " " << "Min:" << Min_R << " " << "Max:" << Max_R << "" << endl;
    writing_file_01 << "#Energy   Ave:" << Sum_E/(max - 1.) << " " << "Min:" << Min_E << " " << "Max:" << Max_E << "" << endl;
    writing_file_01 << "#data number__Range[cm]__Energy[MeV]" << endl;
    for ( i = 0; i < max - 1; i++)
    {
        writing_file_01 << tag[i] << " " << R[i] << " " << E[i] << "" << endl;
    }
    




    //ヒストグラム用データ作成
    ////区間幅0.4
    for ( i = 0; i < max - 1; i++)
    {
        for ( k = 0; k < 40; k++)
        {
            if (R[i] <= d*(k + 1.) && R[i] > d*k)
            {
                H_R[k] = H_R[k] + 1.;/* code */
            }
            else;/* code */
            if (E[i] <= d*(k + 1.) && E[i] > d*k)
            {
                H_E[k] = H_E[k] + 1.;/* code */
            }
            else;/* code */
        }
    }


    cout << "#class value [cm/MeV]_" << "_Count (Range_" << "_sigma(range)_" << "_Count (Energy)_" << "_sigma(energy)" ;
    cout << endl;
    for ( k = 0; k < 40; k++)
    {
        cout << d*k + 0.5*d << "  " << H_R[k] << "  " << sqrt(H_R[k]) << "  " << H_E[k] << "  " << sqrt(H_E[k]) << "  " ;/* code */
        cout << endl;
    }

    ////計算結果をファイルに出力
    ofstream writing_file_02;
    writing_file_02.open("0711_hist01.dat", ios::out);
    writing_file_02 << "#-----output data for hist gram of Range and Energy-----" << endl;
    writing_file_02 << "#Range   Ave:" << Sum_R/(max - 1.) << " " << "Min:" << Min_R << " " << "Max:" << Max_R << "" << endl;
    writing_file_02 << "#Energy   Ave:" << Sum_E/(max - 1.) << " " << "Min:" << Min_E << " " << "Max:" << Max_E << "" << endl;
    writing_file_02 << "#class value [cm/MeV]_" << "_Count (Range)_" << "_sigma(range)_" << "_Count (Energy)_" << "_sigma(energy)" << endl;
    for ( k = 0; k < 40; k++)
    {
        writing_file_02 << d*k + 0.5*d << " " << H_R[k] << " " << sqrt(H_R[k]) << " " << H_E[k] << " " << sqrt(H_E[k]) << "" << endl;
    }
    
    cout << endl;
    cout << "やったぜ。";
    cout << endl;
    cout << endl;

    return 0;
}