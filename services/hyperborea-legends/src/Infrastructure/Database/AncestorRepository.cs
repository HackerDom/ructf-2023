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
    private readonly IDbConnection _db;

    public AncestorRepository(IConfiguration configuration)
    {
        _db = new NpgsqlConnection(configuration["DatabaseConnectionString"]);
    }

    public async Task CreateAsync(Ancestor ancestor)
    {
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
        await _db.ExecuteAsync(query, queryParams);
    }

    public async Task<bool> CheckExistsByIdAsync(Guid id)
    {
        var result = await _db.QueryFirstAsync<int>(
            "SELECT COUNT(*) FROM ancestors WHERE id = @id",
            new { id = id.ToByteArray() }
        );
        return result > 0;
    }

    public async Task AddDirectAncestorToUser(Guid ancestorId, int userId)
    {
        await _db.ExecuteAsync(
            "UPDATE users SET directAncestors = directAncestors || @ancestorId WHERE id = @userId",
            new { ancestorId = ancestorId.ToByteArray(), userId });
    }

    public async Task<Ancestor?> GetByIdAsync(Guid id)
    {
        var ancestor = await _db.QueryFirstOrDefaultAsync<Ancestor>(
            "SELECT * FROM ancestors WHERE id = @id",
            new { id = id.ToByteArray() }
        );
        return ancestor;
    }
}