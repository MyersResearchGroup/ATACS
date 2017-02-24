use work.channel.all;

entity mergexample is
end mergexample;

architecture new_structure of mergexample is
  component winery
    port(wine_to_old_shop, wine_to_new_shop : inout channel);
  end component;

  component merge
    port(X, Y, Z : inout channel);
  end component;

  component patron
    port(wine_buying : inout channel);
  end component;

  signal X, Y, Z : channel := init_channel;
begin
  XYenv : winery port map(wine_to_old_shop => X, wine_to_new_shop => Y);
  theMERGE : merge port map(X => X, Y => Y, Z => Z);
  Zenv : patron port map(wine_buying => Z);
end new_structure;
