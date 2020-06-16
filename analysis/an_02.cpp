#include<iostream>  
#include<cmath>     //数学関数いろいろ
#include<string>    //読み書きに便利らしい
#include<fstream>   //ファイル入出力

using namespace std;

int main(void){
    double len0, h0, len1, h1, t0, t1, cix0_1, ciz0_1, cfx0_1, cfz0_1, cix0_2, ciz0_2, cfx0_2, cfz0_2, cix1_1, ciz1_1, cfx1_1, cfz1_1, cix1_2, ciz1_2, cfx1_2, cfz1_2, cx0_1, cz0_1, cx0_2, cz0_2, cx1_1, cz1_1, cx1_2, cz1_2, ct0, ct1, clx0, clz0, clx1, clz1;
    double tag[50], ix1[50], iz1[50], fx1[50], fz1[50], ix2[50], iz2[50], fx2[50], fz2[50], T[50], P[50], V[50], Rho_eta[50], Rho_air[50];
    double dH = 3.86*pow(10., 4.), p_0 = 5.866168*pow(10.,3.), T_0 = 293.15;
    //dH: エタノールの標準生成エンタルピー[kJ/mol], p_0[Pa],T_0[K]: 20℃における温度と蒸気圧
    double X0[50], Z0[50], X1[50], Z1[50], LX[50], LY[50], LZ[50], Rn[50], En[50], Rc[50], Ec[50];
    double Min_Rn = 0.0, Min_En = 0.0, Max_Rn = 0.0, Max_En = 0.0, Sum_Rn = 0.0, Sum_En = 0.0, Min_Rc = 0.0, Min_Ec = 0.0, Max_Rc = 0.0, Max_Ec = 0.0, Sum_Rc = 0.0, Sum_Ec = 0.0;
    double d = 0.5, H_Rn[50], H_En[50], H_Rc[50], H_Ec[50];
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
        T[i] = 0.0;
        P[i] = 0.0;
        V[i] = 0.0;
        Rho_eta[i] = 0.0;
        Rho_air[i] =0.0;
        
        X0[i] = 0.0;
        Z0[i] = 0.0;
        X1[i] = 0.0;
        Z1[i] = 0.0;

        LX[i] = 0.0;
        LY[i] = 0.0;
        LZ[i] = 0.0;

        Rn[i] = 0.0;
        En[i] = 0.0;
        Rc[i] = 0.0;
        Ec[i] = 0.0;

        H_Rn[i] = 0.0;
        H_En[i] = 0.0;
        H_Rc[i] = 0.0;
        H_Ec[i] = 0.0;
    }
    


    //飛程読み取りデータ読み込み
    in_d = fopen("0711_02.dat", "r");

    for ( i = 0; i < 50; i++)
    {
        j ++;
        if(EOF == fscanf(in_d, "%lf%lf%lf%lf%lf%lf%lf%lf%lf%lf", &tag[i], &ix1[i], &iz1[i], &fx1[i], &fz1[i], &ix2[i], &iz2[i], &fx2[i], &fz2[i], &T[i])) break;
    }
    max = j;

    cout << endl;
    cout << max << " ";
    cout << endl;

    fclose(in_d);

    cout << endl;
    for ( i = 0; i < max - 1; i++)
    {
        cout << tag[i] << " " << ix1[i] << " " << iz1[i] << " " << fx1[i] << " " << fz1[i] << " " << ix2[i] << " " << iz2[i] << " " << fx2[i] << " " << fz2[i] << " " << T[i] << " " << endl;
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
    cout << "-----output calculated range and energy-----" << endl;
    cout << "#data number__Temperature[K]__Range(補正無)[cm]__Energy(補正無)[MeV]__Range(補正有)[cm]__Energy(補正有)[MeV]" << endl;

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
        Rn[i] = sqrt(pow(LX[i], 2.0) + pow(LY[i], 2.0) + pow(LZ[i], 2.0));

        //飛程[cm]から運動エネルギー[MeV]に換算
        En[i] = pow(0.318, -2.0/3.0)*pow(Rn[i], 2.0/3.0);

        //読み込んだ温度のデータからエタノール気体,空気の密度算出
        P[i] = p_0*exp(-(dH/8.31)*((1./T[i]) - (1./T_0)));
        Rho_eta[i] = (4.61*pow(10., -2.)*P[i])/(8.31*T[i]);
        Rho_air[i] = (2.8966*1.013)/(6.022*1.380*T[i]);

        //温度、エタノールの効果を想定し、補正を入れた飛程、エネルギーを算出
        //Rc[i] = (Rho_eta[i]/Rho_air[i])*sqrt(28.966/46.1)*Rn[i];
        Rc[i] = ((273.15 + 15.)/T[i])*Rn[i];
        Ec[i] = pow(0.318, -2.0/3.0)*pow(Rc[i], 2.0/3.0);

        //結果出力
        cout << tag[i] << " " << T[i] << " " << Rn[i] << " " << En[i] << " "<< Rc[i] << " " << Ec[i] << " " << endl;
    }
    
    cout << endl;

    ////最大、最小値、平均値の計算
    Min_Rn = Rn[0];
    Max_Rn = Rn[0];
    Min_En = En[0];
    Max_En = En[0];
    Min_Rc = Rc[0];
    Max_Rc = Rc[0];
    Min_Ec = Ec[0];
    Max_Ec = Ec[0];    
    for ( i = 0; i < max - 1; i++)
    {
        Sum_Rn = Sum_Rn + Rn[i];
        Sum_En = Sum_En + En[i];

        Sum_Rc = Sum_Rc + Rc[i];
        Sum_Ec = Sum_Ec + Ec[i];

        if (Min_Rn > Rn[i]){
            Min_Rn = Rn[i];
        } else;
        if (Max_Rn < Rn[i]){
            Max_Rn = Rn[i];
        } else;
        
        if (Min_En > En[i]){
            Min_En = En[i];
        } else;
        if (Max_En < En[i]){
            Max_En = En[i];
        } else;        

        if (Min_Rc > Rc[i]){
            Min_Rc = Rc[i];
        } else;
        if (Max_Rc < Rc[i]){
            Max_Rc = Rc[i];
        } else;
        
        if (Min_Ec > Ec[i]){
            Min_Ec = Ec[i];
        } else;
        if (Max_Ec < Ec[i]){
            Max_Ec = Ec[i];
        } else;             
    }
    cout << endl;
    cout << "#Range(補正無)   Ave:" << Sum_Rn/(max - 1.) << " " << "Min:" << Min_Rn << " " << "Max:" << Max_Rn << "" << endl;
    cout << "#Energy(補正無)   Ave:" << Sum_En/(max - 1.) << " " << "Min:" << Min_En << " " << "Max:" << Max_En << "" << endl;
    cout << endl;
    cout << "#Range(補正有)   Ave:" << Sum_Rc/(max - 1.) << " " << "Min:" << Min_Rc << " " << "Max:" << Max_Rc << "" << endl;
    cout << "#Energy(補正有)   Ave:" << Sum_Ec/(max - 1.) << " " << "Min:" << Min_Ec << " " << "Max:" << Max_Ec << "" << endl;
    cout << endl;

    //計算結果をファイルに出力
    ofstream writing_file_01;
    writing_file_01.open("0711_result03.dat", ios::out);
    writing_file_01 << "#-----output calculated range and energy-----" << endl;
    writing_file_01 << "#Range   Ave:" << Sum_Rn/(max - 1.) << " " << "Min:" << Min_Rn << " " << "Max:" << Max_Rn << "" << endl;
    writing_file_01 << "#Energy   Ave:" << Sum_En/(max - 1.) << " " << "Min:" << Min_En << " " << "Max:" << Max_En << "" << endl;
    writing_file_01 << "#data number__Temperature[K]__Range(補正無)[cm]__Energy(補正無)[MeV]__Range(補正有)[cm]__Energy(補正有)[MeV]" << endl;
    for ( i = 0; i < max - 1; i++)
    {
        writing_file_01 << tag[i] << " " << Rn[i] << " " << En[i] << " " << Rc[i] << " " << Ec[i] << "" << endl;
    }
    




    //ヒストグラム用データ作成
    ////区間幅0.4
    for ( i = 0; i < max - 1; i++)
    {
        for ( k = 0; k < 30; k++)
        {
            if (Rn[i] <= d*(k + 1.) && Rn[i] > d*k)
            {
                H_Rn[k] = H_Rn[k] + 1.;/* code */
            }
            else;/* code */
            if (En[i] <= d*(k + 1.) && En[i] > d*k)
            {
                H_En[k] = H_En[k] + 1.;/* code */
            }
            else;

            if (Rc[i] <= d*(k + 1.) && Rc[i] > d*k)
            {
                H_Rc[k] = H_Rc[k] + 1.;/* code */
            }
            else;/* code */
            if (Ec[i] <= d*(k + 1.) && Ec[i] > d*k)
            {
                H_Ec[k] = H_Ec[k] + 1.;/* code */
            }
            else;
        }
    }


    cout << "#class value [cm/MeV]__Count(Range)__sigma(R)__Count(Energy)__sigma(E)__Count(Range)__sigma(R)__Count(Energy)__sigma(E)" << endl;
    for ( k = 0; k < 30; k++)
    {
        cout << d*k + 0.5*d << "  " << H_Rn[k] << "  " << sqrt(H_Rn[k]) << "  " << H_En[k] << "  " << sqrt(H_En[k]) << "  " << H_Rc[k] << "  " << sqrt(H_Rc[k]) << "  " << H_Ec[k] << "  " << sqrt(H_Ec[k]) << "  "<< endl;
    }

    ////計算結果をファイルに出力
    ofstream writing_file_02;
    writing_file_02.open("0711_hist04.dat", ios::out);
    writing_file_02 << "#-----output data for hist gram of Range and Energy-----" << endl;
    writing_file_02 << "#Range(補正無)   Ave:" << Sum_Rn/(max - 1.) << " " << "Min:" << Min_Rn << " " << "Max:" << Max_Rn << "" << endl;
    writing_file_02 << "#Energy(補正無)   Ave:" << Sum_En/(max - 1.) << " " << "Min:" << Min_En << " " << "Max:" << Max_En << "" << endl;
    writing_file_02 << "#Range(補正有)   Ave:" << Sum_Rc/(max - 1.) << " " << "Min:" << Min_Rc << " " << "Max:" << Max_Rc << "" << endl;
    writing_file_02 << "#Energy(補正有)   Ave:" << Sum_Ec/(max - 1.) << " " << "Min:" << Min_Ec << " " << "Max:" << Max_Ec << "" << endl;
    writing_file_02 << "#class value [cm/MeV]__Count(Range)__sigma(R)__Count(Energy)__sigma(E)__Count(Range)__sigma(R)__Count(Energy)__sigma(E)" << endl;
    for ( k = 0; k < 30; k++)
    {
        writing_file_02 << d*k + 0.5*d << " " << H_Rn[k] << " " << sqrt(H_Rn[k]) << " " << H_En[k] << " " << sqrt(H_En[k]) << " " << H_Rc[k] << " " << sqrt(H_Rc[k]) << " " << H_Ec[k] << " " << sqrt(H_Ec[k]) << "" << endl;
    }
    
    cout << endl;
    cout << "やったぜ。";
    cout << endl;
    cout << endl;

    return 0;
}