# serial-read.ps1 : 시리얼 포트를 정해진 시간만 읽고 끝낸다. (Windows)
# Claude Code 같은 AI 도구가 업로드 뒤 보드 출력을 확인할 때 쓴다.
#
# 사용법:
#   powershell -ExecutionPolicy Bypass -File tools/serial-read.ps1 -Port COM5 -Seconds 10
#
# i2r PLC(i2r-02/03/04)는 USB-시리얼 칩(CH340C)을 쓴다.
# 이 칩은 DTR/RTS 신호로 보드를 리셋하거나 업로드 모드로 바꾸므로, 읽을 때는 둘 다 끈다.

param(
  [Parameter(Mandatory = $true)][string]$Port,
  [int]$Seconds = 10,
  [int]$Baud = 115200
)

[Console]::OutputEncoding = [System.Text.Encoding]::UTF8

# 업로드 직후에는 포트가 잠깐 바쁠 수 있다. 최대 10초 동안 포트가 나타나기를 기다린다.
$deadline = (Get-Date).AddSeconds(10)
while (-not ([System.IO.Ports.SerialPort]::GetPortNames() -contains $Port)) {
  if ((Get-Date) -gt $deadline) {
    Write-Output "[serial-read] 포트 $Port 를 찾을 수 없습니다. 지금 있는 포트: $([System.IO.Ports.SerialPort]::GetPortNames() -join ', ')"
    exit 1
  }
  Start-Sleep -Milliseconds 300
}

$sp = New-Object System.IO.Ports.SerialPort $Port, $Baud, 'None', 8, 'One'
$sp.Encoding = [System.Text.Encoding]::UTF8
$sp.DtrEnable = $false    # CH340C: DTR/RTS를 켜면 보드가 리셋되거나 업로드 모드로 들어갈 수 있다
$sp.RtsEnable = $false
$sp.ReadTimeout = 200

try {
  $sp.Open()
} catch {
  Write-Output "[serial-read] $Port 를 열 수 없습니다. Arduino IDE 시리얼 모니터 등 다른 프로그램이 쓰고 있는지 확인하세요. ($($_.Exception.Message))"
  exit 1
}

Write-Output "[serial-read] $Port ($Baud) 를 $Seconds 초 동안 읽습니다."
$end = (Get-Date).AddSeconds($Seconds)
try {
  while ((Get-Date) -lt $end) {
    $text = $sp.ReadExisting()
    if ($text) { [Console]::Write($text) }
    Start-Sleep -Milliseconds 50
  }
} finally {
  $sp.Close()
}
Write-Output ""
Write-Output "[serial-read] 끝"
