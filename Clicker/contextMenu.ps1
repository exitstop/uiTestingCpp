# if error execute
# Set-ExecutionPolicy RemoteSigned

$o = new-object -com Shell.Application
$folder = $o.NameSpace("C:\Users\user\source\repos\Clicker\Clicker")
$file=$folder.ParseName("CMakeLists.txt")
$file.Verbs() | select Name
$file.Verbs() | %{ if($_.Name -eq 'Scan for Security Threats') { $_.DoIt() } }