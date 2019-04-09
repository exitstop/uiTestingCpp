# if error execute
# Set-ExecutionPolicy RemoteSigned

$o = new-object -com Shell.Application
$folder = $o.NameSpace("C:\Users\user\source\repos\Clicker\x64\Debug")
$file=$folder.ParseName("Clicker.ilk")
$file.Verbs() | select Name
$file.Verbs() | %{ if($_.Name -eq 'Scan for_Security Threats') { $_.DoIt() } }