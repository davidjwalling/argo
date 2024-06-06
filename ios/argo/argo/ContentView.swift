import SwiftUI

func applicationWillTerminate(_ notification: Notification)
{
    let wrapperItem = wrap_driver();
    let _ = wrapperItem?.stop();
}

struct ContentView: View {
    var body: some View {
        let wrapperItem = wrap_driver();
        Button(action: {
            let _ = wrapperItem?.start();
        }) {
            Text("Start")
                .padding()
                .background(Color.green)
                .foregroundColor(.white)
                .font(.title);
        }
        Button(action : {
            let _ = wrapperItem?.stop();
        }) {
            Text("Stop")
                .padding()
                .background(Color.red)
                .foregroundColor(.white)
                .font(.title);
        }
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
