using System.Text;
using API;
using API.Services;
using Infrastructure.Database;
using Infrastructure.Redis;
using Microsoft.AspNetCore.Authentication.JwtBearer;
using Microsoft.IdentityModel.Tokens;

var builder = WebApplication.CreateBuilder(args);

builder.Services
    .AddAuthentication(options =>
    {
        options.DefaultAuthenticateScheme = JwtBearerDefaults.AuthenticationScheme;
        options.DefaultChallengeScheme = JwtBearerDefaults.AuthenticationScheme;
    })
    .AddJwtBearer(options =>
    {
        options.RequireHttpsMetadata = false;
        options.SaveToken = true;
        options.TokenValidationParameters = new TokenValidationParameters
        {
            ValidateIssuer = false,
            ValidateAudience = false,
            ValidateLifetime = true,
            ValidateIssuerSigningKey = true,
            IssuerSigningKey = new SymmetricSecurityKey(
                Encoding.ASCII.GetBytes(builder.Configuration["JwtSecret"]!)
            )
        };
    });

builder.Services.AddAuthorization();

builder.Services.AddSingleton<IUserRepository, UserRepository>();
builder.Services.AddSingleton<IAncestorRepository, AncestorRepository>();
builder.Services.AddSingleton<IRedisUserRepository, RedisUserRepository>();
builder.Services.AddSingleton<JwtAuthManager>();

builder.Services.AddGrpc();

var app = builder.Build();

app.UseAuthentication();
app.UseAuthorization();

app.MapGrpcService<AuthService>();
app.MapGrpcService<AncestorService>();

app.Run();