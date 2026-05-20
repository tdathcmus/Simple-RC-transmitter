# Simple RC Transmitter
This project features a 4 channel RC transmitter using STM32F103C8T6 Blue Pill and NRF24L01 module.<br>


**Component:<br>**
- STM32F103C8T6 Blue Pill
- NRF24L01 EBYTE E01-2G4M27D
- Joystick module x2
- Battery holder 2x18650
- [DC-DC-Dual-Rail-Power-Supply](https://github.com/tdathcmus/DC-DC-Dual-Rail-Power-Supply-5V-3A-3.3V-1.5A)


**Conect:<br>**

**NRF24:**
- PA3: CE<br>
- PA4: CSN<br>


<img width="779" height="613" alt="image" src="https://github.com/user-attachments/assets/c240a237-14e9-4b5c-b59d-0649f58be6e9" /><br>
**ADC:**<br>
**Using ADC1:** IN1, IN2, IN8, IN9<br>
- Scan Conversion Mode: Enabled<br>
- Continuous Conversion Mode: Enabled<br>


**Left joystick<br>**
- X: PA1 **(Rudder)**<br>
- Y: PA2 **(Throttle)**<br>


**Right joystick<br>**
- X: PB1 **(Roll)**<br>
- Y: PB0 **(Elevator)**<br>

**Power:**<br>
[DC-DC-Dual-Rail-Power-Supply](https://github.com/tdathcmus/DC-DC-Dual-Rail-Power-Supply-5V-3A-3.3V-1.5A)<br>
- Connect the wires from the battery box to a switch and then to the input of the DC-DC board.<br>
- Connect the 5V output to the STM32F103C8T6 Blue Pill and the 3.3V output to the E01-2G4M27D.<br>
<img width="1210" height="750" alt="image" src="https://github.com/user-attachments/assets/5312aed0-416b-452c-a795-4e13195737df" />


<br>**After wiring, soldering the components, and flashing the code onto the STM32F103C8T6 Blue Pill, 3D print the case and assemble the final product**


<img width="2160" height="2160" alt="image" src="https://github.com/user-attachments/assets/c14594e9-28ec-4c64-9902-5200cfae3ba2" />


<img width="2160" height="2160" alt="image" src="https://github.com/user-attachments/assets/73b60354-6f71-47d6-a1ff-56abb7fd717e" />

