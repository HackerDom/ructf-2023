using System.IdentityModel.Tokens.Jwt;
using System.Security.Claims;
using System.Security.Cryptography;
using System.Text;
using Domain;
using Grpc.Core;
using Infrastructure.Database;
using Infrastructure.Redis;
using Microsoft.IdentityModel.Tokens;

namespace API;

public class JwtAuthManager
{
    private readonly int _jwtExpiredMinutes;
    private readonly string _jwtSecret;
    private readonly IRedisUserRepository _redisUserRepository;
    private readonly IUserRepository _userRepository;

    public JwtAuthManager(IUserRepository userRepository, IRedisUserRepository redisUserRepository,
        IConfiguration configuration)
    {
        _userRepository = userRepository;
        _redisUserRepository = redisUserRepository;
        _jwtSecret = configuration["JwtSecret"]!;
        _jwtExpiredMinutes = int.Parse(configuration["JwtExpiredMinutes"]!);
    }

    public async Task<string> Authenticate(string username, string password)
    {
        var user = await _userRepository.GetByUsernameAsync(username);
        if (user == null) throw new RpcException(new Status(StatusCode.NotFound, "User not found"));
        if (user.Password != GetPasswordHash(password))
            throw new RpcException(new Status(StatusCode.Unauthenticated, "Invalid password"));

        var tokenSecret = Encoding.ASCII.GetBytes(_jwtSecret);
        var tokenExpiryDateTime = DateTime.Now.AddMinutes(_jwtExpiredMinutes);

        var securityTokenDescriptor = new SecurityTokenDescriptor
        {
            Subject = new ClaimsIdentity(
                new List<Claim> { new("id", user.Id.ToString()) }
            ),
            Expires = tokenExpiryDateTime,
            SigningCredentials = new SigningCredentials(
                new SymmetricSecurityKey(tokenSecret),
                SecurityAlgorithms.HmacSha256Signature
            )
        };

        var jwtSecurityTokenHandler = new JwtSecurityTokenHandler();

        var securityToken = jwtSecurityTokenHandler.CreateToken(securityTokenDescriptor);
        var token = jwtSecurityTokenHandler.WriteToken(securityToken);

        return token;
    }

    public async Task<User> GetUserByContext(ServerCallContext context)
    {
        var userId = GetUserIdClaim(context);

        var dbUser = await _userRepository.GetByIdAsync(userId);
        if (dbUser == null) throw new RpcException(new Status(StatusCode.NotFound, "User not found."));

        return dbUser;
    }

    public async Task<User?> GetCacheUserByContext(ServerCallContext context)
    {
        var userId = GetUserIdClaim(context);

        return await _redisUserRepository.GetByIdAsync(userId);
    }

    private int GetUserIdClaim(ServerCallContext context)
    {
        if (!context.GetHttpContext().User.HasClaim(c => c.Type == "id"))
            throw new RpcException(new Status(StatusCode.Unauthenticated, "Invalid token."));

        return int.Parse(
            context.GetHttpContext().User.Claims.FirstOrDefault(c => c.Type == "id")?.Value!
        );
    }

    public static string GetPasswordHash(string password)
    {
        var hashBytes = SHA256.HashData(Encoding.UTF8.GetBytes(password));
        return BitConverter.ToString(hashBytes);
    }
}