import 'package:flutter/material.dart';
import 'package:flutter_blue_plus/flutter_blue_plus.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Xiao Control',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: XiaoControlScreen(),
    );
  }
}

class XiaoControlScreen extends StatefulWidget {
  @override
  _XiaoControlScreenState createState() => _XiaoControlScreenState();
}

class _XiaoControlScreenState extends State<XiaoControlScreen> {
  FlutterBluePlus flutterBlue = FlutterBluePlus();
  BluetoothDevice? _connectedDevice;
  BluetoothCharacteristic? _characteristic;
  bool isConnected = false;
  String connectionStatus = 'Disconnected';
  List<BluetoothDevice> savedDevices = []; // List to save devices

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Xiao Control'),
        actions: [
          IconButton(
            icon: Icon(Icons.bluetooth),
            onPressed: () => _scanForDevices(context),
          ),
          IconButton(
            icon: Icon(Icons.list),
            onPressed: () => _showSavedDevices(context), // Navigate to saved devices
          ),
        ],
      ),
      body: Center(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text(
              'Connection Status: $connectionStatus',
              style: TextStyle(fontSize: 18),
            ),
            SizedBox(height: 20),
            GridView.count(
              shrinkWrap: true,
              crossAxisCount: 2,
              children: [
                _buildControlButton(context, 'One', Icons.looks_one, '1'),
                _buildControlButton(context, 'Two', Icons.looks_two, '2'),
                _buildControlButton(context, 'Three', Icons.looks_3, '3'),
                _buildControlButton(context, 'Four', Icons.looks_4, '4'),
              ],
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildControlButton(BuildContext context, String label, IconData icon, String data) {
    return ElevatedButton(
      onPressed: () => _sendDataToAllSavedDevices(data), // Send data to all saved devices
      child: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          Icon(icon, size: 50),
          Text(label),
        ],
      ),
    );
  }

  void _scanForDevices(BuildContext context) {
    showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: Text('Select a Device'),
        content: SingleChildScrollView(
          child: Column(
            children: [
              StreamBuilder<List<ScanResult>>(
                stream: FlutterBluePlus.scanResults,
                builder: (context, snapshot) {
                  if (snapshot.hasData) {
                    List<ScanResult> devices = snapshot.data!;
                    return Column(
                      children: devices.map((result) {
                        return ListTile(
                          title: Text(result.device.name.isNotEmpty
                              ? result.device.name
                              : 'Unnamed device'),
                          onTap: () {
                            FlutterBluePlus.stopScan();
                            _connectToDevice(result.device);
                            Navigator.of(context).pop();
                          },
                          onLongPress: () {
                            setState(() {
                              savedDevices.add(result.device); // Save device to the list
                            });
                            Navigator.of(context).pop();
                          },
                        );
                      }).toList(),
                    );
                  } else {
                    return CircularProgressIndicator();
                  }
                },
              ),
            ],
          ),
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.of(context).pop(),
            child: Text('Cancel'),
          ),
        ],
      ),
    );

    FlutterBluePlus.startScan(timeout: Duration(seconds: 4));
  }

  void _connectToDevice(BluetoothDevice device) async {
    await device.connect();
    setState(() {
      _connectedDevice = device;
      isConnected = true;
      connectionStatus = 'Connected to ${device.name}';
    });

    List<BluetoothService> services = await device.discoverServices();
    services.forEach((service) {
      service.characteristics.forEach((characteristic) {
        _characteristic = characteristic;
      });
    });
  }

  void _sendData(String data) async {
    if (_characteristic != null) {
      await _characteristic!.write(data.codeUnits);
    } else {
      ScaffoldMessenger.of(context).showSnackBar(SnackBar(
        content: Text('Not connected to any device'),
        backgroundColor: Colors.red,
      ));
    }
  }

  void _sendDataToAllSavedDevices(String data) async {
    for (BluetoothDevice device in savedDevices) {
      try {
        await device.connect();
        List<BluetoothService> services = await device.discoverServices();
        for (var service in services) {
          for (var characteristic in service.characteristics) {
            await characteristic.write(data.codeUnits);
          }
        }
        await device.disconnect();
      } catch (e) {
        print("Failed to send data to ${device.name}: $e");
      }
    }
  }

  void _showSavedDevices(BuildContext context) {
    showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: Text('Saved Devices'),
        content: SingleChildScrollView(
          child: Column(
            children: savedDevices.map((device) {
              return ListTile(
                title: Text(device.name),
                trailing: IconButton(
                  icon: Icon(Icons.delete),
                  onPressed: () {
                    setState(() {
                      savedDevices.remove(device); // Remove device from list
                    });
                    Navigator.of(context).pop();
                  },
                ),
                onTap: () {
                  _connectToDevice(device);
                  Navigator.of(context).pop();
                },
              );
            }).toList(),
          ),
        ),
        actions: [
          TextButton(
            onPressed: () => Navigator.of(context).pop(),
            child: Text('Close'),
          ),
        ],
      ),
    );
  }
}