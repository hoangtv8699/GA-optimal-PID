#pragma once
class GA_PID {
private:
	float sumOfWater, sumOfSalt; // tổng muối và nước hiện tại để tính ECt
	float maxExceed; // vọt lố lớn nhất
	float ECtarget; 
	float ECt;
	float sumOfPumpTime; // tổng thời gian bơm
	float waterIn1LAB; // lượng nước có trong 1 lít dung dịch AB
	float saltIn1LAB; // lượng muối có trong 1 lít dung dịch AB
	float sumOfWaterUsed; // tổng lượng nước bơm vào
	float sumOfSaltUsed; // tổng lượng muối bơm vào
	float sumOfMixingTime; // tổng thời gian trộn để trung hòa bể dinh hưỡng sau khi bơm
	float inletWaterPerMin; // lượng nước bơm vào trong 1 phút của máy bơm
	float Kpmax, Kpmin, Kimax, Kimin, Kdmax, Kdmin;
	float gen[30][3]; // 30 gen cho thuật toán GA
	float e0, e1, e2; // sai số cho thuật toán incremental PID

	// hàm tăng giá trị EC theo output của PID
	void getECUp(float output) {

	}
	// hàm giảm giá trị EC theo output của PID
	void getECDown(float output) {

	}
	// hàm thay đổi giá trị EC theo output của PID
	void updateEC(float output) {
		if (output > 0) {
			getECUp(output);
		}
		else
		{
			getECDown(output);
		}
	}
	// hàm khởi tạo các sai số ban đầu cho thuật toán PID
	void initPID() {
		// to do
	}
	// hàm tính toán giá trị output theo incremental PID
	float compute() {
		float output;
		// to do
		return output;
	}
	// hàm lai ghép
	void crossover() {
		// to do
	}
	// hàm đột biến
	void mutation() {
		// to do
	}
	// hàm tính toán GA
	void start() {
		// to do
	}

public:
	// hàm set ECtarget
	void setECtarget(float ECtarget) {
		this->ECtarget = ECtarget;
	}
	// hàm set ECt tại thời điểm t = 0
	void setECt(float sumOfWater, float sumOfSalt) {
		this->sumOfWater = sumOfWater;
		this->sumOfSalt = sumOfSalt;
	}
	// hàm set số lượng nước bơm vào của máy bơm trong 1 phút
	void setInletWaterPerMin(float inletWaterPerMin) {
		this->inletWaterPerMin = inletWaterPerMin;
	}
};
