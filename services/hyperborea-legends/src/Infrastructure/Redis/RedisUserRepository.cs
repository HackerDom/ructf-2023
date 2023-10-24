using System.Text.Json;
using Domain;
using Microsoft.Extensions.Configuration;
using StackExchange.Redis;

namespace Infrastructure.Redis;

public interface IRedisUserRepository
{
    Task<User?> GetByIdAsync(int id);
}

public class RedisUserRepository : IRedisUserRepository
{
    private readonly IDatabase _db;

    public RedisUserRepository(IConfiguration configuration)
    {
        var redis = ConnectionMultiplexer.Connect(configuration["CacheConnectionString"]!);
        _db = redis.GetDatabase();
    }

    public async Task<User?> GetByIdAsync(int id)
    {
        var res = _db.StringGet($"user:{id}");
        if (res.IsNullOrEmpty) return null;
        return await Task.FromResult(JsonSerializer.Deserialize<User>(res!));
    }
}