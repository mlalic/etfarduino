// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#pragma once

#include "EtfArduinoService.h"
#include <memory>
namespace EtfArduinoConfig {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Collections;

	using namespace System::IO::Ports;
	using namespace System::ComponentModel;

	using namespace System::Threading;

	/// <summary>
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void) : service(new EtfArduinoService), devicePorts(gcnew ArrayList())
		{
			InitializeComponent();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if (components)
			{
				delete components;
			}
			delete service;
		}
	private: EtfArduinoService* service;

	private: System::Windows::Forms::Button^  button2;
	private: array<String^>^ serialPorts;
	private: ArrayList^ devicePorts;
	private: System::Windows::Forms::ListBox^  listBoxSerialPorts;
	private: System::Windows::Forms::ListBox^  listBoxDevices;


	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Button^  buttonRegisterDevice;
	private: System::Windows::Forms::Button^  buttonRefreshDevices;


	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->listBoxSerialPorts = (gcnew System::Windows::Forms::ListBox());
			this->listBoxDevices = (gcnew System::Windows::Forms::ListBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->buttonRegisterDevice = (gcnew System::Windows::Forms::Button());
			this->buttonRefreshDevices = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(44, 236);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(210, 23);
			this->button2->TabIndex = 1;
			this->button2->Text = L"Detect ports";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &Form1::button2_Click);
			// 
			// listBoxSerialPorts
			// 
			this->listBoxSerialPorts->FormattingEnabled = true;
			this->listBoxSerialPorts->Location = System::Drawing::Point(44, 44);
			this->listBoxSerialPorts->Name = L"listBoxSerialPorts";
			this->listBoxSerialPorts->Size = System::Drawing::Size(210, 186);
			this->listBoxSerialPorts->TabIndex = 2;
			// 
			// listBoxDevices
			// 
			this->listBoxDevices->FormattingEnabled = true;
			this->listBoxDevices->Location = System::Drawing::Point(341, 44);
			this->listBoxDevices->Name = L"listBoxDevices";
			this->listBoxDevices->Size = System::Drawing::Size(210, 186);
			this->listBoxDevices->TabIndex = 3;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(44, 25);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(157, 13);
			this->label1->TabIndex = 4;
			this->label1->Text = L"Ports with no registered devices";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(341, 25);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(98, 13);
			this->label2->TabIndex = 5;
			this->label2->Text = L"Registered devices";
			// 
			// buttonRegisterDevice
			// 
			this->buttonRegisterDevice->Location = System::Drawing::Point(260, 66);
			this->buttonRegisterDevice->Name = L"buttonRegisterDevice";
			this->buttonRegisterDevice->Size = System::Drawing::Size(75, 43);
			this->buttonRegisterDevice->TabIndex = 6;
			this->buttonRegisterDevice->Text = L"Register device";
			this->buttonRegisterDevice->UseVisualStyleBackColor = true;
			this->buttonRegisterDevice->Click += gcnew System::EventHandler(this, &Form1::buttonRegisterDevice_Click);
			// 
			// buttonRefreshDevices
			// 
			this->buttonRefreshDevices->Location = System::Drawing::Point(344, 236);
			this->buttonRefreshDevices->Name = L"buttonRefreshDevices";
			this->buttonRefreshDevices->Size = System::Drawing::Size(206, 23);
			this->buttonRefreshDevices->TabIndex = 7;
			this->buttonRefreshDevices->Text = L"Refresh registered devices";
			this->buttonRefreshDevices->UseVisualStyleBackColor = true;
			this->buttonRefreshDevices->Click += gcnew System::EventHandler(this, &Form1::buttonRefreshDevices_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(601, 281);
			this->Controls->Add(this->buttonRefreshDevices);
			this->Controls->Add(this->buttonRegisterDevice);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->listBoxDevices);
			this->Controls->Add(this->listBoxSerialPorts);
			this->Controls->Add(this->button2);
			this->Name = L"Form1";
			this->Text = L"Form1";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
				 DetectSerialPorts();
				 PopulateSerialListBox();
			 }
	private: void DetectSerialPorts() {
				 try {
					 serialPorts = System::IO::Ports::SerialPort::GetPortNames();
				 } catch (Exception^ exc) {
					 MessageBox::Show("Problem detecting ports...");
					 return;
				 }
			 }
	private: void PopulateSerialListBox() {
				 listBoxSerialPorts->Items->Clear();
				 for each (String^ serial in serialPorts) {
					 // Don't add the serial port to the list if there's
					 // already a registered device detected on that port
					 if (!devicePorts->Contains(serial))
						listBoxSerialPorts->Items->Add(serial);
				 }
			 }
	private: void AddToDeviceList(int deviceId, String^ portName) {
				 if (listBoxSerialPorts->Items->Contains(portName)) {
					 listBoxSerialPorts->Items->Remove(portName);
				 }
				 devicePorts->Add(portName);
				 listBoxDevices->Items->Add(String::Format("etfarduino (Device {0}) @ {1}", deviceId, portName));
			 }
	private: delegate void ListUpdateDelegate(int deviceId, String^ portName);
	private: void DeviceDetectThread() {
				 // Go through IDs 0 to 10 and check if there are registered devices...
				 EtfArduinoService service;
				 for (int i = 0; i <= 10; ++i) {
					 TCHAR serialPortName[10];
					 if (service.CheckDeviceRegistered(i, serialPortName)) {
						 // Delegate the form update to the GUI thread
						 this->BeginInvoke(gcnew ListUpdateDelegate(this, &Form1::AddToDeviceList), 
										   i,
										   gcnew String(serialPortName));
					 }
				 }
			 }
	private: void InitDeviceDetectThread() {
				 ThreadStart^ start = gcnew ThreadStart(this, &Form1::DeviceDetectThread);
				 Thread^ t = gcnew Thread(start);
				 t->Start();
			 }
	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) {
				 DetectSerialPorts();
				 PopulateSerialListBox();
				 InitDeviceDetectThread();
			 }
	 private: bool RegisterDevice(String^ serialPort) {
				  // Need to convert the managed String into an array of TCHAR
				  array<wchar_t>^ chars = serialPort->ToCharArray();
				  if (chars->Length >= 10) {
					  MessageBox::Show("Port name too long to register device.",
						 "Notification",
						 MessageBoxButtons::OK,
						 MessageBoxIcon::Exclamation);
					  return false;

				  }
				  TCHAR serialPortName[10];
				  memset(serialPortName, 0, sizeof serialPortName);
				  for (size_t i = 0; i < chars->Length; ++i) {
					  serialPortName[i] = chars[i];
				  }
				  UINT deviceId = -1;
				  if (service->RegisterDevice(deviceId, serialPortName)) {
					  MessageBox::Show("Successfully registered the device",
						  "Information",
						  MessageBoxButtons::OK,
						  MessageBoxIcon::Information);
					  AddToDeviceList(deviceId, serialPort);
					  return true;
				  } else {
					  MessageBox::Show("Device registration failed.",
						 "Notification",
						 MessageBoxButtons::OK,
						 MessageBoxIcon::Exclamation);
					  return false;
				  }
			  }
private: System::Void buttonRegisterDevice_Click(System::Object^  sender, System::EventArgs^  e) {
			 if (listBoxSerialPorts->SelectedIndex == -1) {
				 MessageBox::Show("Select a port from the list.",
					 "Notification",
					 MessageBoxButtons::OK,
					 MessageBoxIcon::Exclamation);
			 } else {
				 RegisterDevice((String^)listBoxSerialPorts->SelectedItem);
			 }
		 }
private: System::Void buttonRefreshDevices_Click(System::Object^  sender, System::EventArgs^  e) {
			 listBoxDevices->Items->Clear();
			 devicePorts->Clear();
			 InitDeviceDetectThread();
		 }
};
}

