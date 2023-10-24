using System.Data;
using Dapper;
using Domain;
using Microsoft.Extensions.Configuration;
using Npgsql;

namespace Infrastructure.Database;

public interface IUserRepository
{
    Task<User?> GetByUsernameAsync(string username);
    Task<User?> GetByIdAsync(int id);
    Task<int> CreateAsync(string username, string password, string species);
    Task<bool> CheckUsernameExistsAsync(string username);
}

public class UserRepository : IUserRepository
{
    private readonly IDbConnection _db;

    public UserRepository(IConfiguration configuration)
    {
        _db = new NpgsqlConnection(configuration["DatabaseConnectionString"]);
    }

    public async Task<bool> CheckUsernameExistsAsync(string username)
    {
        var result = await _db.QueryFirstAsync<int>(
            "SELECT COUNT(*) FROM users WHERE username = @username",
            new { username }
        );
        return result > 0;
    }

    public async Task<int> CreateAsync(string username, string password, string type)
    {
        var id = await _db.QueryFirstAsync<int>(
            "INSERT INTO users (username, password, species) VALUES (@username, @password, @type) RETURNING id",
            new { username, password, type }
        );
        return id;
    }

    public async Task<User?> GetByIdAsync(int id)
    {
        var user = await _db.QueryFirstOrDefaultAsync<User>(
            "SELECT * FROM users WHERE id = @id",
            new { id }
        );
        return user;
    }

    public async Task<User?> GetByUsernameAsync(string username)
    {
        var user = await _db.QueryFirstOrDefaultAsync<User>(
            "SELECT * FROM users WHERE username = @username",
            new { username }
        );
        return user;
    }
}