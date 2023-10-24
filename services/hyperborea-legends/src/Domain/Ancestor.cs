namespace Domain;

public class Ancestor
{
    private byte[] id { get; set; }

    public Guid Id
    {
        get => new(id);
        set => id = value.ToByteArray();
    }

    public string Name { get; set; } = null!;
    public string Description { get; set; } = null!;
    public SpeciesType Species { get; set; }
    public string BurialPlace { get; set; } = null!;
    public int OwnerId { get; set; }
}