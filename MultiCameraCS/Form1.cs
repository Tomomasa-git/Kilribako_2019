using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Threading;
using System.Drawing.Imaging;

using Teli.TeliCamAPI.NET;
using Teli.TeliCamAPI.NET.Utility;


namespace MultiCameraCS
{
    public partial class Form1 : Form
    {
        private const int MaxCameraCount = 4;

        private CameraSystem camSystem = null;
        private CameraDevice[] arrayCamDevice = null;

        private bool isOpen = false;
        private bool isAcquisitionActive = false;
        private int camCount = 0;
        private int selectedTabIndex = 0;

        private TextBox[] arrayTextBoxInfoCam = null;
        private TextBox[] arrayTextBoxFps = null;
        private TextBox[] arrayTextBoxBlockId = null;
        private PictureBox[] arrayPictureBox = null;
        private Label[] arrayLabelErrorMessage = null;
        private NumericUpDown[] arrayNumericUpDownGain = null;
        private NumericUpDown[] arrayNumericUpDownFrameRate = null;
        private TabPage[] arrayTabPage = null;
        private int[] arrayFpsCount = null;

        // Display Thread
        Thread displayThread = null;
        private bool bTerminatedDispThread = false;

        private delegate void DispTextBoxDelegate(TextBox textBox, string text);
        private delegate void DispLabelDelegate(Label label, string text, bool bVisible);
        private delegate void DispBmpDelegate(int cameraIndex, Bitmap bmp);


        public Form1()
        {
            InitializeComponent();
        }

        private void DispTextBox(TextBox textBox, string text)
        {
            // Don't do anything if the form's handle hasn't been created 
            // or the form has been disposed.
            if (!this.IsHandleCreated && !this.IsDisposed) return;

            // Invoke an anonymous method on the thread of the form.
            if (textBox.InvokeRequired)
            {
                this.BeginInvoke(new DispTextBoxDelegate(this.DispTextBox), new object[] { textBox, text });
                return;
            }

            textBox.Text = text;
            textBox.Update();
        }

        private void DispLabel(Label label, string text, bool bVisible)
        {
            // Don't do anything if the form's handle hasn't been created 
            // or the form has been disposed.
            if (!this.IsHandleCreated && !this.IsDisposed) return;

            // Invoke an anonymous method on the thread of the form.
            if (label.InvokeRequired)
            {
                this.BeginInvoke(new DispLabelDelegate(this.DispLabel), new object[] { label, text, bVisible });
                return;
            }

            label.Text = text;
            label.Visible = bVisible;
            label.Update();
        }

        ////////////////////////////////////////////////////////////////////////////////
        // Image display update thread.
        //  *Image update cycle is 50msec.
        //  *FPS update cycle is 2sec.
        // 
        private void ThreadDisplay()
        {
            double fps;
            System.Diagnostics.Stopwatch sw = new System.Diagnostics.Stopwatch();

            sw.Reset();
            sw.Start();

            while (!this.IsDisposed && !bTerminatedDispThread)
            {
                if (this.isAcquisitionActive)
                {
                    // Check interval
                    if (sw.ElapsedMilliseconds > 2000)
                    {
                        // FPS update
                        for (int i = 0; i < this.camCount; i++)
                        {
                            fps = (double)this.arrayFpsCount[i] / (double)sw.ElapsedMilliseconds * 1000.0;
                            DispTextBox(this.arrayTextBoxFps[i], fps.ToString("F1"));

                            Interlocked.Exchange(ref this.arrayFpsCount[i], 0);
                        }
                        sw.Reset();
                        sw.Start();
                    }

                    // Update image
                    if (selectedTabIndex < camCount)
                    {
                        UpdateImage(selectedTabIndex);
                    }
                }

                // Image update cycle 50msec (20 Image / sec)
                Thread.Sleep(50);
            }

            sw.Stop();
        }


        private void Initialize()
        {
            CamApiStatus status;

            if (this.camSystem == null)
            {
                // Create instance.
                this.camSystem = new CameraSystem();

                this.arrayCamDevice = new CameraDevice[MaxCameraCount];

                // Initialize Api
                status = this.camSystem.Initialize();
                if (status != CamApiStatus.Success)
                {
                    MessageBox.Show("Initialize error!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                // Prepare table for converting YUV image to BGRA.
                status = CameraUtility.PrepareLUT();
                if (status != CamApiStatus.Success)
                {
                    MessageBox.Show("PrepareLUT error!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                this.arrayTextBoxInfoCam = new TextBox[MaxCameraCount];
                this.arrayTextBoxInfoCam[0] = this.textBoxInfoCam1;
                this.arrayTextBoxInfoCam[1] = this.textBoxInfoCam2;
                this.arrayTextBoxInfoCam[2] = this.textBoxInfoCam3;
                this.arrayTextBoxInfoCam[3] = this.textBoxInfoCam4;

                this.arrayTextBoxFps = new TextBox[MaxCameraCount];
                this.arrayTextBoxFps[0] = this.textBoxFps1;
                this.arrayTextBoxFps[1] = this.textBoxFps2;
                this.arrayTextBoxFps[2] = this.textBoxFps3;
                this.arrayTextBoxFps[3] = this.textBoxFps4;

                this.arrayTextBoxBlockId = new TextBox[MaxCameraCount];
                this.arrayTextBoxBlockId[0] = this.textBoxBlockId1;
                this.arrayTextBoxBlockId[1] = this.textBoxBlockId2;
                this.arrayTextBoxBlockId[2] = this.textBoxBlockId3;
                this.arrayTextBoxBlockId[3] = this.textBoxBlockId4;

                this.arrayNumericUpDownGain = new NumericUpDown[MaxCameraCount];
                this.arrayNumericUpDownGain[0] = this.numericUpDownGain1;
                this.arrayNumericUpDownGain[1] = this.numericUpDownGain2;
                this.arrayNumericUpDownGain[2] = this.numericUpDownGain3;
                this.arrayNumericUpDownGain[3] = this.numericUpDownGain4;

                this.arrayNumericUpDownFrameRate = new NumericUpDown[MaxCameraCount];
                this.arrayNumericUpDownFrameRate[0] = this.numericUpDownFrameRate1;
                this.arrayNumericUpDownFrameRate[1] = this.numericUpDownFrameRate2;
                this.arrayNumericUpDownFrameRate[2] = this.numericUpDownFrameRate3;
                this.arrayNumericUpDownFrameRate[3] = this.numericUpDownFrameRate4;

                this.arrayPictureBox = new PictureBox[MaxCameraCount];
                this.arrayPictureBox[0] = this.pictureBox1;
                this.arrayPictureBox[1] = this.pictureBox2;
                this.arrayPictureBox[2] = this.pictureBox3;
                this.arrayPictureBox[3] = this.pictureBox4;

                this.arrayLabelErrorMessage = new Label[MaxCameraCount];
                this.arrayLabelErrorMessage[0] = this.labelErrorMessage1;
                this.arrayLabelErrorMessage[1] = this.labelErrorMessage2;
                this.arrayLabelErrorMessage[2] = this.labelErrorMessage3;
                this.arrayLabelErrorMessage[3] = this.labelErrorMessage4;

                this.arrayTabPage = new TabPage[MaxCameraCount];
                this.arrayTabPage[0] = this.tabPage1;
                this.arrayTabPage[1] = this.tabPage2;
                this.arrayTabPage[2] = this.tabPage3;
                this.arrayTabPage[3] = this.tabPage4;

                this.arrayFpsCount = new int[MaxCameraCount];

            }
        }

        ////////////////////////////////////////////////////////////////////////////////
        // Close All cameras
        //
        private bool CloseAllCameras()
        {
            for (int camNo = 0; camNo < MaxCameraCount; camNo++)
            {
                if (this.arrayCamDevice[camNo] != null)
                {
                    this.arrayCamDevice[camNo].Close();
                    this.arrayCamDevice[camNo] = null;
                }
            }

            this.isOpen = false;

            return true;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // Display the Bitmap image in the PictureBox.
        //
        void DispBitmap(int cameraIndex, Bitmap bmp)
        {
            // Don't do anything if the form's handle hasn't been created 
            // or the form has been disposed.
            if (!this.IsHandleCreated && !this.IsDisposed) return;

            // Invoke an anonymous method on the thread of the form.
            if (this.arrayPictureBox[cameraIndex].InvokeRequired)
            {
                this.BeginInvoke(new DispBmpDelegate(this.DispBitmap), new object[] { cameraIndex, bmp });
                return;
            }

            this.arrayPictureBox[cameraIndex].Image = bmp;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // Update Image.
        //  *Get the current image from the device.
        //  *Create Bitamap data.
        //
        void UpdateImage(int cameraIndex)
        {
            CamApiStatus status;
            int bufferIndex = 0;

            Bitmap bmp = null;
            BitmapData bitmapData = null;
            CameraImageInfo imageInfo = null;

            status = this.arrayCamDevice[cameraIndex].camStream.GetCurrentBufferIndex(out bufferIndex);
            if ((status != CamApiStatus.Success) || (bufferIndex == -1))
            {
                return;
            }

            status = this.arrayCamDevice[cameraIndex].camStream.LockBuffer(bufferIndex, ref imageInfo);
            if (status != CamApiStatus.Success)
            {
                return;
            }

            try
            {
                if (imageInfo.Status != CamApiStatus.Success)
                {
                    return;
                }

                // Create Bitmap data.
                unsafe
                {
                    if (imageInfo.PixelFormat == CameraPixelFormat.Mono8)
                    {
                        bmp = new Bitmap(imageInfo.SizeX, imageInfo.SizeY, System.Drawing.Imaging.PixelFormat.Format8bppIndexed);
                        // Set palette in bitmap
                        ColorPalette monoPalette = bmp.Palette;
                        for (int i = 0; i <= 255; i++)
                        {
                            monoPalette.Entries[i] = Color.FromArgb(i, i, i);
                        }
                        bmp.Palette = monoPalette;

                        bitmapData = bmp.LockBits(new Rectangle(0, 0, imageInfo.SizeX, imageInfo.SizeY), ImageLockMode.WriteOnly, PixelFormat.Format8bppIndexed);
                        CameraUtility.CopyMemoryT(bitmapData.Scan0, imageInfo.BufferPointer, (imageInfo.SizeX * imageInfo.SizeY));
                    }
                    else
                    {
                        bmp = new Bitmap(imageInfo.SizeX, imageInfo.SizeY, System.Drawing.Imaging.PixelFormat.Format24bppRgb);

                        bitmapData = bmp.LockBits(new Rectangle(0, 0, imageInfo.SizeX, imageInfo.SizeY), ImageLockMode.WriteOnly, PixelFormat.Format24bppRgb);
                        CameraUtility.ConvertImage(DstPixelFormat.BGR24, imageInfo.PixelFormat, true, bitmapData.Scan0, imageInfo.BufferPointer, imageInfo.SizeX, imageInfo.SizeY);
                    }

                    bmp.UnlockBits(bitmapData);
                }
            }
            finally
            {
                this.arrayCamDevice[cameraIndex].camStream.UnlockBuffer(bufferIndex);
            }

            DispBitmap(cameraIndex, bmp);

            GC.Collect();
        }

        /////////////////////////////////////////////////////////
        // Image Acquierd event callback function.
        // 
        void camStream_ImageAcquired(CameraStream sender, ImageAcquiredEventArgs e)
        {
            CameraDevice parent = sender.Parent;
            int cameraIndex = -1;

            for (int i = 0; i < this.camCount; i++)
            {
                if (parent == this.arrayCamDevice[i])
                {
                    cameraIndex = i;
                    break;
                }
            }
            if (cameraIndex == -1)
            {
                return;
            }

            // Disp Block ID
            DispTextBox(this.arrayTextBoxBlockId[cameraIndex], e.ImageInfo.BlockID.ToString());

            // Increment fps count
            Interlocked.Increment(ref this.arrayFpsCount[cameraIndex]);

        }

        /////////////////////////////////////////////////////////
        // Image Error event callback function.
        // 
        void camStream_ImageErrorReceived(CameraStream sender, ImageErrorReceivedEventArgs e)
        {
            CameraDevice parent = sender.Parent;
            int cameraIndex = -1;

            for (int i = 0; i < this.camCount; i++)
            {
                if (parent == this.arrayCamDevice[i])
                {
                    cameraIndex = i;
                    break;
                }
            }
            if (cameraIndex == -1)
            {
                return;
            }

            DispLabel(this.arrayLabelErrorMessage[cameraIndex], e.ErrorStatus.ToString(), true);
        }

        /////////////////////////////////////////////////////////
        // Buffer Busy event callback function.
        // 
        void camStream_BufferBusyReceived(CameraStream sender, BufferBusyReceivedEventArgs e)
        {
            CameraDevice parent = sender.Parent;
            int cameraIndex = -1;

            for (int i = 0; i < this.camCount; i++)
            {
                if (parent == this.arrayCamDevice[i])
                {
                    cameraIndex = i;
                    break;
                }
            }
            if (cameraIndex == -1)
            {
                return;
            }

            DispLabel(this.arrayLabelErrorMessage[cameraIndex], "BusyBuffer", true);
        }

        /////////////////////////////////////////////////////////
        // Update camera list.
        //  *Get number of cameras.
        //  *Open the all camera.
        //  *Add event handler.
        // 
        private void UpdateCameraList()
        {
            CamApiStatus status;
            int count;

            this.Enabled = false;

            try
            {
                // Stop receiving image
                CloseAllCameras();

                // Get number of cameras
                status = this.camSystem.GetNumOfCameras(out count);
                if (status != CamApiStatus.Success)
                {
                    MessageBox.Show("UpdateCameraList error : " + status.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                if (count > MaxCameraCount)
                {
                    // if detected too much cameras. use first MaxCameraCount cameras.
                    this.camCount = MaxCameraCount;
                }
                else
                {
                    this.camCount = (int)count;
                }

                for (int camNo = 0; camNo < this.camCount; camNo++)
                {
                    // Create TeliCameraDevice object.
                    status = this.camSystem.CreateDeviceObject(camNo, ref this.arrayCamDevice[camNo]);
                    if (status != CamApiStatus.Success)
                    {
                        MessageBox.Show("CreateDeviceObject error : " + status.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    }

                    status = this.arrayCamDevice[camNo].Open();
                    if (status != CamApiStatus.Success)
                    {
                        MessageBox.Show("Open error : " + status.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    }

                    // Set TriggerMode false, in this sample code.
                    status = this.arrayCamDevice[camNo].camControl.SetTriggerMode(false);     // Freerun mode

                    // Set AcquisitionFrameRateControl Manual, in this sample code.
                    if (this.arrayCamDevice[camNo].CamType == CameraType.TypeU3v)
                    {
                        status = this.arrayCamDevice[camNo].camControl.SetAcquisitionFrameRateControl(CameraAcqFrameRateCtrl.Manual);
                    }

                    // Add a new event handler.
                    this.arrayCamDevice[camNo].camStream.ImageAcquired += new ImageAcquiredEventHandler(camStream_ImageAcquired);
                    this.arrayCamDevice[camNo].camStream.ImageErrorReceived += new ImageErrorReceivedEventHandler(camStream_ImageErrorReceived);
                    this.arrayCamDevice[camNo].camStream.BufferBusyReceived += new BufferBusyReceivedEventHandler(camStream_BufferBusyReceived);
                }

                textBoxNumOfCamera.Text = this.camCount.ToString();

                UpdateCameraInfo();
                UpdateGain();
                UpdateFrameRate();

                if (this.camCount > 0)
                {
                    this.isOpen = true;
                }
            }
            finally
            {
                this.Enabled = true;
            }
        }

        ////////////////////////////////////////////////////////////////////////////////
        // Start of image acquisition.
        //  *Open & start of the Stream.
        //  *Start of image display thread.
        //
        private void StartAcquisition()
        {
            CamApiStatus status;
            System.Int32 maxPayloadSize;

            if (this.isAcquisitionActive)
            {
                return;
            }

            for (int camNo = 0; camNo < this.camCount; camNo++)
            {
                DispTextBox(this.arrayTextBoxFps[camNo], "0");
                DispLabel(this.arrayLabelErrorMessage[camNo], "", false);

                status = this.arrayCamDevice[camNo].camStream.Open(out maxPayloadSize);
                if (status != CamApiStatus.Success)
                {
                    MessageBox.Show("camStream.Open error : " + status.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                status = this.arrayCamDevice[camNo].camStream.Start();
                if (status != CamApiStatus.Success)
                {
                    MessageBox.Show("camStream.Start error : " + status.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }

                Interlocked.Exchange(ref this.arrayFpsCount[camNo], 0);
            }

            this.bTerminatedDispThread = false;
            this.displayThread = new Thread(new ThreadStart(ThreadDisplay));
            this.displayThread.IsBackground = true;
            this.displayThread.Start();

            this.isAcquisitionActive = true;
        }

        ////////////////////////////////////////////////////////////////////////////////
        // Stop of image acquisition.
        //  *Stop & close of the Stream.
        //  *Terminate of image display thread.
        //
        private void StopAcquisition()
        {
            CamApiStatus status;

            if (!this.isAcquisitionActive)
            {
                return;
            }

            this.bTerminatedDispThread = true;

            for (int camNo = 0; camNo < this.camCount; camNo++)
            {
                status = this.arrayCamDevice[camNo].camStream.Stop();
                if (status != CamApiStatus.Success)
                {
                    MessageBox.Show("camStream.Stop error : " + status.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }

            this.displayThread.Join();

            for (int camNo = 0; camNo < this.camCount; camNo++)
            {
                status = this.arrayCamDevice[camNo].camStream.Close();
                if (status != CamApiStatus.Success)
                {
                    MessageBox.Show("camStream.Close error : " + status.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }

            this.isAcquisitionActive = false;
        }

        private void Terminate()
        {
            StopAcquisition();

            CloseAllCameras();

            camSystem.Terminate();
        }

        private void UpdateControl()
        {
            UpdateGain();
            UpdateFrameRate();

            if (this.isOpen)
            {
                if (this.isAcquisitionActive)
                {
                    this.buttonStartAcquisition.Enabled = false;
                    this.buttonStopAcquisition.Enabled = true;
                    this.buttonUpdateCameraList.Enabled = false;
                }
                else
                {
                    this.buttonStartAcquisition.Enabled = true;
                    this.buttonStopAcquisition.Enabled = false;
                    this.buttonUpdateCameraList.Enabled = true;
                }
            }
            else
            {
                this.buttonStartAcquisition.Enabled = false;
                this.buttonStopAcquisition.Enabled = false;
                this.buttonUpdateCameraList.Enabled = true;
            }
        }

        /////////////////////////////////////////////////////////
        // Get camera information. (*All camera)
        // 
        private void UpdateCameraInfo()
        {
            CamApiStatus status;

            for (int camNo = 0; camNo < MaxCameraCount; camNo++)
            {
                if (camNo < this.camCount)
                {
                    CameraInfo info = null;
                    status = this.arrayCamDevice[camNo].GetInformation(ref info);
                    if (status != CamApiStatus.Success)
                    {
                        MessageBox.Show("GetInformation.Stop error : " + status.ToString(), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                        return;
                    }
                    this.arrayTextBoxInfoCam[camNo].Text = "ModelName : " + info.ModelName + " , SerialNo : " + info.SerialNumber;
                    this.arrayTabPage[camNo].Text = info.ModelName + " (" + info.SerialNumber + ")";

                }
                else
                {
                    this.arrayTextBoxInfoCam[camNo].Text = "";
                    this.arrayTabPage[camNo].Text = "-";
                }

                this.arrayTextBoxBlockId[camNo].Text = "";
                this.arrayTextBoxFps[camNo].Text = "";
                this.arrayPictureBox[camNo].Image = null;
                this.arrayLabelErrorMessage[camNo].Visible = false;

            }
        }

        /////////////////////////////////////////////////////////
        // Get Gain feature. (*All camera)
        // 
        private void UpdateGain()
        {
            CamApiStatus status;

            for (int camNo = 0; camNo < MaxCameraCount; camNo++)
            {
                if (camNo < this.camCount)
                {
                    double gain;
                    double min;
                    double max;

                    status = this.arrayCamDevice[camNo].camControl.GetGainMinMax(out min, out max);
                    if (status == CamApiStatus.Success)
                    {
                        this.arrayNumericUpDownGain[camNo].Minimum = (decimal)min;
                        this.arrayNumericUpDownGain[camNo].Maximum = (decimal)max;
                    }
                    status = this.arrayCamDevice[camNo].camControl.GetGain(out gain);
                    if (status == CamApiStatus.Success)
                    {
                        this.arrayNumericUpDownGain[camNo].Value = (decimal)gain;
                    }

                    this.arrayNumericUpDownGain[camNo].Enabled = true;
                }
                else
                {
                    this.arrayNumericUpDownGain[camNo].Enabled = false;
                }
            }
        }

        /////////////////////////////////////////////////////////
        // Get Framerate feature. (*All camera)
        // 
        private void UpdateFrameRate()
        {
            CamApiStatus status;

            for (int camNo = 0; camNo < MaxCameraCount; camNo++)
            {
                if ((!this.isAcquisitionActive) && (camNo < this.camCount))
                {
                    double frameRate;
                    double min;
                    double max;

                    status = this.arrayCamDevice[camNo].camControl.GetAcquisitionFrameRateMinMax(out min, out max);
                    if (status == CamApiStatus.Success)
                    {
                        this.arrayNumericUpDownFrameRate[camNo].Minimum = (decimal)min;
                        this.arrayNumericUpDownFrameRate[camNo].Maximum = (decimal)max;
                    }
                    status = this.arrayCamDevice[camNo].camControl.GetAcquisitionFrameRate(out frameRate);
                    if (status == CamApiStatus.Success)
                    {
                        this.arrayNumericUpDownFrameRate[camNo].Value = (decimal)frameRate;
                    }

                    this.arrayNumericUpDownFrameRate[camNo].Enabled = true;
                }
                else
                {
                    this.arrayNumericUpDownFrameRate[camNo].Enabled = false;
                }
            }
        }

        /////////////////////////////////////////////////////////
        // Set Gain value.
        // 
        private void SetGain(int cameraIndex, double gain)
        {
            if (!this.isOpen)
            {
                return;
            }

            CamApiStatus status;
            double min;
            double max;

            status = this.arrayCamDevice[cameraIndex].camControl.GetGainMinMax(out min, out max);
            if (status != CamApiStatus.Success)
            {
                return;
            }

            if (gain < min)
            {
                gain = min;
            }
            if (gain > max)
            {
                gain = max;
            }

            status = this.arrayCamDevice[cameraIndex].camControl.SetGain(gain);
            if (status != CamApiStatus.Success)
            {
                return;
            }

            UpdateGain();
        }

        /////////////////////////////////////////////////////////
        // Set Framerate value.
        // 
        private void SetFrameRate(int cameraIndex, double frameRate)
        {
            if (!this.isOpen)
            {
                return;
            }

            CamApiStatus status;
            double min;
            double max;

            status = this.arrayCamDevice[cameraIndex].camControl.GetAcquisitionFrameRateMinMax(out min, out max);
            if (status != CamApiStatus.Success)
            {
                return;
            }

            if (frameRate < min)
            {
                frameRate = min;
            }
            if (frameRate > max)
            {
                frameRate = max;
            }

            status = this.arrayCamDevice[cameraIndex].camControl.SetAcquisitionFrameRate(frameRate);
            if (status != CamApiStatus.Success)
            {
                return;
            }

            UpdateFrameRate();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            Initialize();
        }

        private void buttonUpdateCameraList_Click(object sender, EventArgs e)
        {
            UpdateCameraList();
            UpdateControl();
        }

        private void buttonStartAcquisition_Click(object sender, EventArgs e)
        {
            StartAcquisition();
            UpdateControl();
        }

        private void buttonStopAcquisition_Click(object sender, EventArgs e)
        {
            StopAcquisition();
            UpdateControl();
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            Terminate();
        }

        private void numericUpDownGain_ValueChanged(object sender, EventArgs e)
        {
            NumericUpDown upDown = sender as NumericUpDown;

            if (sender == this.numericUpDownGain1)
            {
                SetGain(0, (double)upDown.Value);
            }
            else if (sender == this.numericUpDownGain2)
            {
                SetGain(1, (double)upDown.Value);
            }
            else if (sender == this.numericUpDownGain3)
            {
                SetGain(2, (double)upDown.Value);
            }
            else if (sender == this.numericUpDownGain4)
            {
                SetGain(3, (double)upDown.Value);
            }
            else
            {
                // Do nothing
            }
        }

        private void numericUpDownFrameRate_ValueChanged(object sender, EventArgs e)
        {
            NumericUpDown upDown = sender as NumericUpDown;

            if (sender == this.numericUpDownFrameRate1)
            {
                SetFrameRate(0, (double)upDown.Value);
            }
            else if (sender == this.numericUpDownFrameRate2)
            {
                SetFrameRate(1, (double)upDown.Value);
            }
            else if (sender == this.numericUpDownFrameRate3)
            {
                SetFrameRate(2, (double)upDown.Value);
            }
            else if (sender == this.numericUpDownFrameRate4)
            {
                SetFrameRate(3, (double)upDown.Value);
            }
            else
            {
                // Do nothing
            }
        }

        private void tabControl1_SelectedIndexChanged(object sender, EventArgs e)
        {
            selectedTabIndex = tabControl1.SelectedIndex;
        }

    }
}
