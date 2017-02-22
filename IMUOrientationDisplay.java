import com.sun.j3d.utils.geometry.*;
import com.sun.j3d.utils.universe.*;

import java.awt.Dimension;
import java.awt.Toolkit;
import javax.media.j3d.*;
import javax.swing.JFrame;
import javax.swing.WindowConstants;
import javax.vecmath.*;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.util.Scanner;
import org.eclipse.paho.client.mqttv3.IMqttDeliveryToken;
import org.eclipse.paho.client.mqttv3.MqttCallback;
import org.eclipse.paho.client.mqttv3.MqttClient;
import org.eclipse.paho.client.mqttv3.MqttConnectOptions;
import org.eclipse.paho.client.mqttv3.MqttException;
import org.eclipse.paho.client.mqttv3.MqttMessage;

public class IMUOrientationDisplay extends JFrame implements MqttCallback{
	
	MqttClient myClient;
	MqttConnectOptions connOpt;

//	static final String BROKER_URL = "tcp://192.168.0.222:1883";
	String BROKER_URL;
	static final String M2MIO_THING = "test";
	
	private static final int periodT = 40;    //1 over frequency, doesn't need to match Edison's
	
	private float q0;
	private float q1;
	private float q2;
	private float q3;
	private float q0Old;
	private double roll;
	private double pitch;
	private double yaw;
	
	private double axis1Phi;
	private double axis1Theta;
	private double axis2Phi;
	private double axis2Theta;
	
	private static final long serialVersionUID = 1L; //Eclipse issues warnings without this...
	private static int screenWidth;
	private static int screenHeight;
	TransformGroup rotateBox;
	TransformGroup rotateBoxInit;
	TransformGroup rotateAxis;
	TransformGroup rotateAxisInit;
	int degrees;
	
	private int messageCount;
	
	private Scanner ipRead;
	
	
	@Override
	public void connectionLost(Throwable t) {
		System.out.println("Connection lost!");
		// code to reconnect to the broker would go here if desired
	}
	public void runClient() {
		// setup MQTT Client
		String clientID = M2MIO_THING;
		connOpt = new MqttConnectOptions();
		
		connOpt.setCleanSession(true);
		connOpt.setKeepAliveInterval(30);
		
		// Connect to Broker
		try {
			myClient = new MqttClient(BROKER_URL, clientID);
			myClient.setCallback(this);
			myClient.connect(connOpt);
		} catch (MqttException e) {
			e.printStackTrace();
			System.exit(-1);
		}
		
		System.out.println("Connected to " + BROKER_URL);

		String myTopic = M2MIO_THING;

		//subscribe to topic
			try {
				int subQoS = 0;
				myClient.subscribe(myTopic, subQoS);
			} catch (Exception e) {
				e.printStackTrace();
			}
		
		// disconnect
		/*
		try {
			// wait to ensure subscribed messages are delivered
				Thread.sleep(10000);
			myClient.disconnect();
		} catch (Exception e) {
			e.printStackTrace();
		}
		*/
	}

	public void deliveryComplete(IMqttDeliveryToken arg0) {
	}

	
private BranchGroup getContent(){
	
	BranchGroup branchGroup = new BranchGroup();
	branchGroup = addLighting(branchGroup);
	rotateBox = new TransformGroup(new Transform3D());
	rotateBox.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);
	
	rotateBoxInit = new TransformGroup(new Transform3D());
	rotateBoxInit.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);
	
	Box myBox = new Box(0.6f,0.2f,0.4f,new Appearance());
	myBox = assignMaterialToBox(myBox);
	//Transform3D originalBoxTran = new Transform3D();
	rotateBoxInit.addChild(myBox);
	rotateBox.addChild(rotateBoxInit);

	
	//part below here is new!!!!!
	rotateAxis = new TransformGroup(new Transform3D());
	rotateAxis.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);
	
	rotateAxisInit = new TransformGroup(new Transform3D());
	rotateAxisInit.setCapability(TransformGroup.ALLOW_TRANSFORM_WRITE);
	
	Box myAxis = new Box(1.0f,0.05f,0.05f,new Appearance());
	myAxis = assignMaterialToBox(myAxis);
	//Transform3D originalBoxTran = new Transform3D();
	rotateAxisInit.addChild(myAxis);
	rotateAxis.addChild(rotateAxisInit);

	
	branchGroup.addChild(rotateAxis);
	
	branchGroup.addChild(rotateBox);
	
	
	
	return branchGroup;
}


private BranchGroup addLighting(BranchGroup branchGroup){
	BoundingSphere bounds = new BoundingSphere(new Point3d(0,0,0), 400);
    Vector3f direction = new Vector3f(0,0,-7); Color3f color = new Color3f(1.5f,1.5f,4.5f);
    DirectionalLight light = new DirectionalLight(color, direction);
    light.setInfluencingBounds(bounds);
    Color3f color2 = new Color3f(2,8,1); Vector3f direction2 = new Vector3f(0,0,9);
    DirectionalLight light2 = new DirectionalLight(color2,direction2);
    light2.setInfluencingBounds(bounds);
    branchGroup.addChild(light); branchGroup.addChild(light2); return branchGroup;
}
	
private Box assignMaterialToBox(Box myBox){  //doesn't exactly work as intended, but still needed
	Appearance objectAppearance = new Appearance();
    objectAppearance.setCapability(Appearance.ALLOW_MATERIAL_WRITE);
    Material objectMaterial = new Material();
    objectAppearance.setMaterial(objectMaterial);
    myBox.setAppearance(objectAppearance); return myBox;
}
	

public IMUOrientationDisplay(int appWidth, int appHeight){
	Canvas3D thisCanvas = new Canvas3D(SimpleUniverse.getPreferredConfiguration());
	//thisCanvas.addActionListener(this); 
	thisCanvas.setSize(appWidth,appHeight);

	SimpleUniverse universe = new SimpleUniverse(thisCanvas);
	BranchGroup branchGroup = getContent();
	universe.addBranchGraph(branchGroup);
	universe.getViewingPlatform().setNominalViewingTransform();

	getContentPane().add(thisCanvas, "Center"); //nothing will work without this line!!!
}

private void rotateBox(Matrix3f rotateMat, int iniOrNot){
	Transform3D rotate = new Transform3D();
	rotate.set(rotateMat);
	if(iniOrNot==1)
	rotateBox.setTransform(rotate);
	else
	rotateBoxInit.setTransform(rotate);
}

private void rotateBoxEuler(Vector3d EulerVec, int iniOrNot){
	Transform3D rotate = new Transform3D();
	rotate.setEuler(EulerVec);
	if(iniOrNot==1)
	rotateBox.setTransform(rotate);
	else
	rotateBoxInit.setTransform(rotate);  //sets initial transform      TODO
}


private void rotateAxisEuler(Vector3d EulerVec, int iniOrNot){
	Transform3D rotate = new Transform3D();
	rotate.setEuler(EulerVec);
	if(iniOrNot==1)
	rotateAxis.setTransform(rotate);
	else
	rotateAxisInit.setTransform(rotate);  //sets initial transform      TODO
}



private static void screenSize(){
	Dimension size = Toolkit.getDefaultToolkit().getScreenSize();
	if(size.getWidth()>0.0&&size.getHeight()>0.0)
	screenWidth = (int)size.getWidth();
	screenHeight = (int)size.getHeight();
}

private void getMosquittoIP(){
	int degrees = 0;
	q0 = 0.0f; q1 = 0.0f; q2 = 0.0f; q3 = 0.0f;
	roll = 0.0; pitch = 0.0; yaw = 0.0;
	
	setInitialRotation();
	
	
	String location = "lastUrl.txt";
	File urlLocation = new File(location);
	if(!urlLocation.exists()){
		try {
			urlLocation.createNewFile();
			System.out.println("Url Saved To:" + urlLocation.getAbsolutePath());
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
	
	Scanner scanUrl;
	String locationEdison;
	locationEdison = "";
	try {
		scanUrl = new Scanner(urlLocation);
		if(scanUrl.hasNext()){
			System.out.println("Last Edison URL Used:");
			String lastUsedUrl = scanUrl.nextLine();
			System.out.println(lastUsedUrl);
			
			ipRead = new Scanner(System.in);
			System.out.println("Edison IP:");
			locationEdison = ipRead.nextLine();
			//tcp://192.168.0.222:1883   <<<should look like this
			String locationEd = "tcp://"+ locationEdison +":1883";
			System.out.println(locationEd);
			BROKER_URL = locationEd;
//			ipRead.close();//DONT for some reason it seems closing one System.in Scanner breaks all System.in Scanners
			
			if(!locationEdison.equals(lastUsedUrl)){
				//TODO replace past URL with Current One in text File
				System.out.println("Now:");
				System.out.println(locationEdison);
				try {
					PrintWriter printToFile = new PrintWriter(location, "UTF-8");
					printToFile.println(locationEdison);
					printToFile.close();
				} catch (FileNotFoundException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (UnsupportedEncodingException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
			else{
				try {
					ipRead = new Scanner(System.in);
					System.out.println("Edison IP:");
					locationEdison = ipRead.nextLine();
					//tcp://192.168.0.222:1883   <<<should look like this
					String locationEd = "tcp://"+ locationEdison +":1883";
					System.out.println(locationEd);
					BROKER_URL = locationEd;
					PrintWriter printToFile = new PrintWriter(location, "UTF-8");
					printToFile.println(locationEdison);
					printToFile.close();
				} catch (FileNotFoundException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (UnsupportedEncodingException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
			scanUrl.close();
		} catch (FileNotFoundException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		}

	messageCount=0;
}

private void closeOrZero(){
	Scanner cOrZ = new Scanner(System.in);
	System.out.println("Type z/c to zero or close");
	String cZ = cOrZ.nextLine();
	if(cZ.equals("z")||cZ.equals("Z")){
		setInitialRotation();
		closeOrZero();
	}
	else if(cZ.equals("c")||cZ.equals("C")){
		closeAll();
	}
	else
	{
	closeOrZero();	
	}
}

private void closeAll(){
	try {
		myClient.disconnect();
	} catch (MqttException e) {
		// TODO Auto-generated catch block
		e.printStackTrace();
		try {
			Thread.sleep(10000);
		} catch (InterruptedException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		closeAll();
	}
}




public static void main(String[] args){
	try{screenSize();int width = screenWidth/5; int height = screenHeight*4/10;
	IMUOrientationDisplay IMUdisp = new IMUOrientationDisplay(width, height);
	IMUdisp.setSize(width,height);
	IMUdisp.setLocation(0,0);
	//main.setLocation(screenWidth*4/5,0);  //if want the screen to open to the right of screen
	IMUdisp.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);
	IMUdisp.setVisible(true);
	IMUdisp.getMosquittoIP();
	IMUdisp.runClient();
	
	IMUdisp.closeOrZero();
	IMUdisp.setVisible(false);
	System.exit(0);
}catch(Exception e)	{e.printStackTrace();}
}

//only need to change the constructMatrix(rotation) method below
//and the periodT (=1/frequency) (not Edison's) up at the top


private Matrix3f constructMatrix(int degrees, float q0, float q1, float q2, float q3){
/*	
	//Degrees to Elurer for constant Horizontal rotation:
	float cos = (float) Math.cos(((double)degrees)/180.0*Math.PI);
	float sin = (float) Math.sin(((double)degrees)/180.0*Math.PI);
	Matrix3f rotateMat = new Matrix3f(cos,0.0f,-sin,0.0f,1.0f,0.0f,sin,0.0f,cos);
*/
	
	//Quaternion to Euler: (trying to follow http://fabiensanglard.net/doom3_documentation/37726-293748.pdf)
	//float x = q0; float y = q1; float z = q2; float w = q3;
	//float x = q0; float y = q1; float z = q3; float w = q2; //better!
	//float x = q0; float y = q2; float z = q1; float w = q3; //no!
	//float x = q0; float y = q2; float z = q3; float w = q1; //no
	//float x = q0; float y = q3; float z = q1; float w = q2; //no
	//float x = q0; float y = q3; float z = q2; float w = q1; //no
	
	//float x = q1; float y = q0; float z = q2; float w = q3; ehh nah?
	//float x = q1; float y = q0; float z = q3; float w = q2; looks nice but No
	//float x = q1; float y = q3; float z = q0; float w = q2;
	float x = q1; float y = q3; float z = q2; float w = q0; //okay
	//float x = q1; float y = q2; float z = q0; float w = q3;
	//float x = q1; float y = q2; float z = q3; float w = q0;
	
	//float x = q2; float y = q1; float z = q3; float w = q0;
	//float x = q2; float y = q1; float z = q0; float w = q3;
	//float x = q2; float y = q3; float z = q1; float w = q0;
	//float x = q2; float y = q3; float z = q0; float w = q1;
	//float x = q2; float y = q0; float z = q1; float w = q3;
	//float x = q2; float y = q0; float z = q3; float w = q1;
	
	//float x = q3; float y = q0; float z = q1; float w = q2;
	//float x = q3; float y = q0; float z = q2; float w = q1;
	//float x = q3; float y = q1; float z = q0; float w = q2;
	//float x = q3; float y = q1; float z = q2; float w = q0;
	//float x = q3; float y = q2; float z = q1; float w = q0;
	//float x = q3; float y = q2; float z = q0; float w = q1;
	Matrix3f rotateMat = new Matrix3f(1.0f-2.0f*y*y-2.0f*z*z, 2.0f*x*y+2.0f*w*z, 2.0f*x*z - 2.0f*w*y,
									  2.0f*x*y - 2.0f*w*z, 1.0f- 2.0f*x*x-2.0f*z*z, 2.0f*y*z+2.0f*w*x,
									  2.0f*x*z+2.0f*w*y, 2.0f*y*z - 2.0f*w*x, 1.0f - 2.0f*x*x - 2.0f*y*y);
	
	return rotateMat;
}


public void messageArrived(String arg0, MqttMessage arg1) throws Exception {
	
	String message = new String(arg1.getPayload());
	double messageD = 0.0;
	
//	System.out.println(message);
	
	if(messageCount>0)
		try{
		messageD = Double.parseDouble(message);
		}
		catch(Exception e){
			e.printStackTrace();
			messageCount=0;
		}
	
	if(messageCount==1){
		roll = messageD;
		messageCount = 0;
	}
	if(message.equals("R"))
		messageCount = 1;
	if(messageCount==2){
		pitch = messageD;
		messageCount = 0;
	}
	if(message.equals("P"))
		messageCount = 2;
	
	if(messageCount == 8){
		axis2Theta = messageD;
		messageCount = 0;
		System.out.println(axis1Phi);
		System.out.println(axis1Theta);
		System.out.println(axis2Phi);
		System.out.println(axis2Theta);
		
		Vector3d EulerVecAxis = new Vector3d(0, axis2Theta, axis2Phi);
		rotateAxisEuler(EulerVecAxis, 1);  //for Euler
//		Vector3d EulerVecAxisTest = new Vector3d(0, testTheta, testPhi); this seems correct
	}
	
	if(messageCount == 7){
		axis2Phi = messageD;
		messageCount = 8;
	}
	
	if(messageCount == 6){
		axis1Theta = messageD;
		messageCount = 7;
	}
	
	
	if(messageCount == 5){
		axis1Phi = messageD;
		messageCount = 6;
	}
	if(message.equals("A"))
		messageCount = 5;
	
	if(messageCount==3){
		yaw = messageD;	
		messageCount = 0;
//		System.out.println("r"+roll);
//		System.out.println("p"+pitch);
//		System.out.println("y"+yaw);
		
		Vector3d EulerVec = new Vector3d(roll, yaw, pitch);
		rotateBoxEuler(EulerVec, 1);  //for Euler
	}
	if(message.equals("Y"))
		messageCount = 3;	
	
	/*
	if(message.equals("A")){
		messageCount=1;
		System.out.println("HELLO WORLD INDEED!");	
	}
	else{
		try{
			messageD = Double.parseDouble(message);
		}
		catch(Exception e){
			e.printStackTrace();
			messageCount=0;
		}
		
		if(messageCount==3){
			System.out.println("y"+message);
			yaw = messageD;
			Vector3d EulerVec = new Vector3d(roll, yaw, pitch);
			rotateBoxEuler(EulerVec, 1);  //for Euler
			messageCount = 0;
		}
		if(messageCount==2){
			System.out.println("p"+message);
			pitch = messageD;
			messageCount = 3;
		}
		if(messageCount==1){
			System.out.println("r"+message);
			roll = messageD;
			messageCount = 2;
		}
	}*/
}

public void setInitialRotation(){
	Vector3d EulerVec0 = new Vector3d(-roll, -yaw, -pitch); //<-for initially zeroing
	rotateBoxEuler(EulerVec0, 0); //<-for initially zeroing
}

}



//TODO Need to go back eventually and delete tons of unnecessary things
