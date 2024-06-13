import SwiftUI

extension String {
    var lineCount: Int {
        return components(separatedBy: CharacterSet.newlines).count
    }
}

struct ContentView: View {
    @State private var editText: String = ""
    @State private var logContent: String = "I1001 Argo Experimental [0.X]\n"
    @State private var isKeyDown: Bool = false
    @FocusState private var isTextFieldFocused: Bool
    let maxLogLines = 2000
    @State private var scrollProxy: ScrollViewProxy? = nil

    var body: some View {
        VStack {
            ScrollViewReader { proxy in
                ScrollView(.vertical) {
                    Text(logContent)
                        .frame(maxWidth: .infinity, alignment: .leading)
                        .multilineTextAlignment(.leading)
                        .padding()
                        .font(.system(.body, design: .monospaced))
                        .lineLimit(maxLogLines)
                }
                .onAppear {
                    DispatchQueue.main.asyncAfter(deadline: .now()+0.5) {
                        scrollProxy = proxy
                    }
                }
            }
            Spacer()
            TextField("Enter a command", text: $editText)
                .textFieldStyle(RoundedBorderTextFieldStyle())
                .padding()
                .focused($isTextFieldFocused)
                .disableAutocorrection(true)
                .autocapitalization(.none)
        }
        .onAppear {
            DispatchQueue.main.asyncAfter(deadline: .now()+1.0) {
                self.isTextFieldFocused = true
            }
            let wrapperItem = wrap_driver();
            let _ = wrapperItem?.start();
        }
        .onReceive(NotificationCenter.default.publisher(for: UIResponder.keyboardWillHideNotification)) { _ in
            if self.isKeyDown {
                self.handleEnterKeyPressed()
                self.isKeyDown = false
            } else {
                self.isKeyDown = true
            }
        }
    }

    private func handleEnterKeyPressed() {
        logContent.append("\(editText)\n")
        while logContent.lineCount > maxLogLines {
            logContent = logContent.components(separatedBy: "\n").dropFirst().joined(separator: "\n")
        }
        scrollProxy?.scrollTo(0, anchor: .bottom)
        editText = ""
        self.isTextFieldFocused = true
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
