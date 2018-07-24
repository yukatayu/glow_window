# Lib glow window

# Abstract
Win32APIを使用し、ウィンドウを光らせるDLL

# Usage
`init`, `setWindowSize`の後は、
移動時には`setWindowPos`を使い、リサイズ時のみ`setWindowSize`を使うこと。
`setWindowSize`は影の再計算が発生するので重い。
