#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>
using namespace std;



class GA_PID {
private:
	float originalSumOfWater, originalSumOfSalt; // tổng muối(mg) và nước(l) khởi tạo ban đầu
	float ECtarget;
	float sumOfWater, sumOfSalt; // tổng muối(mg) và nước(l) hiện tại để tính ECt
	float maxExceed; // vọt lố lớn nhất
	float ECt;
	float convergingTime; // tổng thời gian hội tụ
	float sumOfPumpTime; // tổng thời gian bơm(s)
	float waterIn1LAB; // lượng nước có trong 1 lít dung dịch AB
	float saltIn1LAB; // lượng muối có trong 1 lít dung dịch AB
	float sumOfWaterUsed; // tổng lượng nước(l) bơm vào
	float sumOfSaltUsed; // tổng lượng muối(mg) bơm vào
	float sumOfMixingTime; // tổng thời gian trộn để trung hòa bể dinh hưỡng sau khi bơm
	float inletWaterPerMin; // lượng nước(l) bơm vào trong 1 phút của 1 van điện từ
	float wattage; // công suất điện của van điện từ (watt)
	float Kpmax, Kpmin, Kimax, Kimin, Kdmax, Kdmin;
	float NST[30][5]; // quẩn thể 30 NST cho thuật toán GA
	float e0, e1, e2; // sai số cho thuật toán incremental PID
	float electrictPrice; // giá tiền điện (kWh)
	float waterPrice; // giá tiền nước (m3)
	float saltPrice; // giá tiền dinh dưỡng

	int errorStop = 10000;

	// sắp xếp theo hàm thích nghi
	void Sort() {
		for (int i = 0; i < 30; i++) {
			for (int j = i; j < 30; j++) {
				if (NST[i][3] > NST[j][3]) {
					swap(NST[i], NST[j]);
				}
			}
		}
	}
	// kiểm tra có trùng lặp
	bool checkSame() {
		for (int i = 0; i < 29; i++) {
			if (NST[29][0] == NST[i][0] && NST[29][1] == NST[i][1] && NST[29][2] == NST[i][2]) {
				return true;
			}
		}
		for (int i = 0; i < 28; i++) {
			if (NST[28][0] == NST[i][0] && NST[28][1] == NST[i][1] && NST[28][2] == NST[i][2]) {
				return true;
			}
		}
		return false;
	}
	// hàm tăng giá trị EC theo output của PID
	void getECUp(float output) {
		// cập nhất giá trị muối và nước sau khi bơm
		sumOfWater += output * (inletWaterPerMin / 60) * waterIn1LAB * 2;
		sumOfSalt += output * (inletWaterPerMin / 60) * saltIn1LAB * 2;
		// cập nhật tổng thời gian bơm
		sumOfPumpTime += output;
		// cập nhật tổng lượng muối và nước sử dụng
		sumOfWaterUsed += output * (inletWaterPerMin / 60) * waterIn1LAB * 2;
		sumOfSaltUsed += output * (inletWaterPerMin / 60) * saltIn1LAB * 2;
	}
	// hàm giảm giá trị EC theo output của PID
	void getECDown(float output) {
		// cập nhất giá trị nước sau khi bơm
		sumOfWater += output * (inletWaterPerMin / 60) * waterIn1LAB;
		// cập nhật tổng thời gian bơm
		sumOfPumpTime += output;
		// cập nhật tổng lượng nước sử dụng
		sumOfWaterUsed += output * (inletWaterPerMin / 60) * waterIn1LAB ;
	}
	// hàm thay đổi giá trị EC theo output của PID
	void updateEC(float output) {
		if (output > 0) {
			getECUp(output);
		}
		else
		{
			getECDown(abs(output));
		}
	}

	float max(float a, float b) {
		return a > b ? a : b;
	}
	// hàm khởi tạo các sai số ban đầu cho thuật toán PID
	void initPID() {
		sumOfSalt = originalSumOfSalt;
		sumOfWater = originalSumOfWater;
		ECt = sumOfSalt / (sumOfWater * 640);
		e0 = abs(ECt - ECtarget);
		e1 = e2 = 0;
		maxExceed = 0;
		sumOfPumpTime = 0;
		sumOfWaterUsed = 0;
		sumOfSaltUsed = 0;
	}
	// hàm tính toán giá trị output theo incremental PID
	float compute(int i) {
		float output;
		output = NST[i][0] * (e0 - e1) + NST[i][1] * e0 + NST[i][2] * (e0 - 2 * e1 + e2);
		return output;
	}
	// hàm thích nghi
	float fitness(int i) {
		float fit = 0;
		initPID();
		int convergingTime = 0;
		while (abs(e0 * errorStop) > 1 && convergingTime < 10000) {
			float output = compute(i);
			updateEC(output);
			ECt = sumOfSalt / (sumOfWater * 640);
			e2 = e1;
			e1 = e0;
			e0 = ECtarget - ECt;
			if (e0 * e1 < 0) {
				maxExceed = max(maxExceed, abs(e0));
			}
			convergingTime++;
		}
		fit += countElectrictPrice(sumOfPumpTime);
		fit += countWaterPrice(sumOfWaterUsed);
		fit += saltPrice * sumOfSaltUsed + convergingTime * 100;
		//fit += maxExceed * 1000;
		if (sumOfWaterUsed < 1500) {
			int a = sumOfWaterUsed;
		}
		NST[i][4] = sumOfWaterUsed;
		return fit;
	}
	// tính tiền điện từ thời gian bơm của máy bơm
	float countElectrictPrice(float sumOfPumpTime) {
		float totalUse = (wattage / 1000) * (sumOfPumpTime / 3600);
		return totalUse * electrictPrice;
	}
	// tính tiền nước từ thời gian bơm của máy bơm
	float countWaterPrice(float sumOfWaterUsed) {
		return (sumOfWaterUsed / 1000) * waterPrice;
 	}
	// hàm lựa chọn
	int choose() {
		float s = 0;
		float rangeXi[31] = { 0 };
		float maxi = NST[29][3];
		for (int i = 0; i < 30; i++) {
			//s += maxi / NST[i][3];
			s += NST[i][3];
		}
		
		for (int i = 1; i < 31; i++) {
			//rangeXi[i] = rangeXi[i - 1] + maxi / NST[i - 1][3];
			rangeXi[i] = rangeXi[i - 1] + NST[i - 1][3];
		}

		float a = random(0, s);
		for (int i = 1; i < 31; i++) {
			if (rangeXi[i - 1] <= a && a <= rangeXi[i]) {
				return i - 1;
			}
		}
	}
	// hàm lai ghép
	void crossover() {
		int a, b;
		//do {
			a = choose();
			b = choose();
			/*int a = rand() % 30;
			int b = rand() % 30;*/
			NST[28][0] = NST[a][0];
			NST[28][1] = NST[b][1];
			NST[28][2] = NST[b][2];
			NST[29][0] = NST[b][0];
			NST[29][1] = NST[a][1];
			NST[29][2] = NST[a][2];
			/*NST[29][0] = (NST[a][0] + NST[b][0]) / 2;
			NST[29][1] = (NST[a][1] + NST[b][1]) / 2;
			NST[29][2] = (NST[a][2] + NST[b][2]) / 2;*/
			
		//} while (checkSame());

		NST[28][3] = fitness(28);
		NST[29][3] = fitness(29);
		Sort();
	}
	// hàm đột biến
	void mutation() {
		do {
			int i = rand() % 30;
			int k = rand() % 3;
			int max, min;
			switch (k)
			{
			case 0:
				min = Kpmin;
				max = Kpmax;
				break;
			case 1:
				min = Kimin;
				max = Kimax;
				break;
			case 2:
				min = Kdmin;
				max = Kdmax;
				break;
			}

			NST[i][k] += 0.5;
			if (NST[i][k] > max) {
				NST[i][k] -= 1;
			}
			if (NST[28][0] < 0 || NST[29][0] < 0) {
			Sort();
			}
			NST[i][3] = fitness(i);
		} while (checkSame());
		Sort();
	}
	//hàm random float
	float random(float min, float max) {
		
		float scale = rand() / (float)RAND_MAX;
		return min + scale * (max - min);
	}
	// hàm khởi tạo quẩn thể, giá trị muối nước và các tham số cần thiết ban đầu
	void init() {
		for (int i = 0; i < 30; i++) {
			NST[i][0] = random(Kpmin, Kpmax);
			NST[i][1] = random(Kimin, Kimax);
			NST[i][2] = random(Kdmin, Kdmax);
			NST[i][3] = fitness(i);
		}
		Sort();
	}
	// hàm cập nhật quần thể mới
	void update() {
		float rand;
		for (int i = 0; i < 20; i++) {
			rand = random(0, 1);
			if (0 <= rand && rand <= 0.6) {
				crossover();
			}
			else if (0.6 < rand && rand <= 0.7) {
				mutation();
			}
		}
	}
	

public:
	// hàm set ECtarget
	void setECtarget(float ECtarget) {
		this->ECtarget = ECtarget;
	}
	// hàm set ECt tại thời điểm t = 0
	void setECt(float sumOfWater, float sumOfSalt) {
		originalSumOfWater = sumOfWater;
		originalSumOfSalt = sumOfSalt;
	}
	// hàm set số lượng nước bơm vào của van trong 1 phút
	void setInletWaterPerMin(float inletWaterPerMin) {
		this->inletWaterPerMin = inletWaterPerMin;
	}
	// hàm set giá trị muối và nước trong 1l AB
	void setECAB(float waterIn1LAB, float saltIn1LAB) {
		this->waterIn1LAB = waterIn1LAB;
		this->saltIn1LAB = saltIn1LAB;
	}
	// hàm set tối đa thời gian bơm
	void setWattage(float wattage) {
		this->wattage = wattage;
	}
	// hàm set max min của kp, ki, kd
	void setMaxMinPID(float Kpmax, float Kpmin, float Kimax, float Kimin, float Kdmax, float Kdmin) {
		this->Kpmax = Kpmax;
		this->Kpmin = Kpmin;
		this->Kimax = Kimax;
		this->Kimin = Kimin;
		this->Kdmax = Kdmax;
		this->Kdmin = Kdmin;
	}
	// hàm set giá tiền diện, nước, muối
	void setPrice(float electrictPrice, float waterPrice, float saltPrice) {
		this->electrictPrice = electrictPrice;
		this->waterPrice = waterPrice;
		this->saltPrice = saltPrice;
	}

	// hàm tính toán GA
	void start(float ECtarget, float sumOfWater, float sumOfSalt, float inletWaterPerMin, float wattage, float waterIn1LAB, float saltIn1LAB,
		float Kpmax, float Kpmin, float Kimax, float Kimin, float Kdmax, float Kdmin, float electrictPrice, float waterPrice, float saltPrice) {
		setECtarget(ECtarget);
		setECt(sumOfWater, sumOfSalt);
		setInletWaterPerMin(inletWaterPerMin);
		setECAB(waterIn1LAB, saltIn1LAB);
		setWattage(wattage);
		setMaxMinPID(Kpmax, Kpmin, Kimax, Kimin, Kdmax, Kdmin);
		setPrice(electrictPrice, waterPrice, saltPrice);
		init();
		print();
		for (int i = 0; i < 100; i++) {
			cout << i << "\n";
			//cout << NST[0][0] << " " << NST[0][1] << " " << NST[0][2] << " " << NST[0][3] << "\n";
			update();
			
		}
		print();
	}

	//hàm in quần thể
	void print() {
		for (int i = 0; i < 30; i++) {
			cout << NST[i][0] << " " << NST[i][1] << " " << NST[i][2] << " " << NST[i][3] << " " << NST[i][4] << "\n";
		}
	}
	
};
