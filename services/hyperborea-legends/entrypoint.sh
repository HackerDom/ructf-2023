# fill JwtSecret field in appsettings.json
cp appsettings.json app.json
sed -i "s/\"JwtSecret\": \"\"/\"JwtSecret\": \"$(openssl rand -hex 20)\"/" app.json
cp app.json appsettings.json
dotnet API.dll
