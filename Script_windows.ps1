try {
    function getAllFile([string]$path) {
    $fc = new-object -com scripting.filesystemobject
    $folder = $fc.getfolder($path)
    Write-Host "Folder Name="$folder.Name "Path="$path
    foreach ($i in $folder.files) {
    Write-Host "`nFile Name::" $i.Name
    Write-Host "File Path::" $i.Path
    }
    #foreach ($i in $folder.subfolders) {
    #getAllFile($i.path)
    #}
    }
    $path =$args[0]
    getAllFile($path)
    }
    catch {
    Write-Host "`n Error:: $($_.Exception.Message)" -ForegroundColor Red -BackgroundColor Yellow
    }