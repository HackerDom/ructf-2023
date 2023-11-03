using System.Data;
using Dapper;
using Domain;
using Microsoft.Extensions.Configuration;
using Npgsql;

namespace Infrastructure.Database;

public interface IAncestorRepository
{
    Task<Ancestor?> GetByIdAsync(Guid id);
    Task CreateAsync(Ancestor ancestor);
    Task<bool> CheckExistsByIdAsync(Guid id);

    Task AddDirectAncestorToUser(Guid ancestorId, int userId);
}

public class AncestorRepository : IAncestorRepository
{
    private readonly string _connString;

    public AncestorRepository(IConfiguration configuration)
    {
        _connString = configuration["DatabaseConnectionString"]!;
    }

    public async Task CreateAsync(Ancestor ancestor)
    {
        await using var db = new NpgsqlConnection(_connString);
        const string query =
            "INSERT INTO ancestors (id, name, description, species, burialPlace, ownerId) VALUES (@id, @name, @description, @species, @burialPlace, @ownerId)";
        var queryParams = new
        {
            id = ancestor.Id.ToByteArray(),
            name = ancestor.Name,
            description = ancestor.Description,
            species = ancestor.Species,
            burialPlace = ancestor.BurialPlace,
            ownerId = ancestor.OwnerId
        };
        await db.ExecuteAsync(query, queryParams);
    }

    public async Task<bool> CheckExistsByIdAsync(Guid id)
    {
        await using var db = new NpgsqlConnection(_connString);
        var result = await db.QueryFirstAsync<int>(
            "SELECT COUNT(*) FROM ancestors WHERE id = @id",
            new { id = id.ToByteArray() }
        );
        return result > 0;
    }

    public async Task AddDirectAncestorToUser(Guid ancestorId, int userId)
    {
        await using var db = new NpgsqlConnection(_connString);
        await db.ExecuteAsync(
            "UPDATE users SET directAncestors = directAncestors || @ancestorId WHERE id = @userId",
            new { ancestorId = ancestorId.ToByteArray(), userId });
    }

    public async Task<Ancestor?> GetByIdAsync(Guid id)
    {
        await using var db = new NpgsqlConnection(_connString);
        var ancestor = await db.QueryFirstOrDefaultAsync<Ancestor>(
            "SELECT * FROM ancestors WHERE id = @id",
            new { id = id.ToByteArray() }
        );
        return ancestor;
    }
}