#include "data_seed.h"
#include "profile_manager.h"
#include <string.h>

/* Adiciona dados básicos ao banco de dados caso eles não estejam lá ainda. */
void seed_database() {
    Profile profile;

    // Cria o arquivo de banco caso não exista
    init_profile_db();

    strcpy(profile.name, "Ariel");
    strcpy(profile.surename, "Versace");
    strcpy(profile.email, "ariel@gg.com");
    strcpy(profile.picture, "ariel.jpg");
    strcpy(profile.address, "Campinas");
    strcpy(profile.education, "Arte");
    strcpy(profile.skills, "Cantar, cortar o cabelo");
    memset(profile.experiences, 0, 640);
    strcpy(profile.experiences[0], "RPDRP S11");
    add_profile(&profile);

    strcpy(profile.name, "Sharon");
    strcpy(profile.surename, "Needles");
    strcpy(profile.email, "sharon@gg.com");
    strcpy(profile.picture, "sharon.jpg");
    strcpy(profile.address, "Sorocaba");
    strcpy(profile.education, "Farmácia");
    strcpy(profile.skills, "Terror, dançar, atuar");
    memset(profile.experiences, 0, 640);
    strcpy(profile.experiences[0], "RPDRP S4");
    add_profile(&profile);
    
    strcpy(profile.name, "Aquaria");
    strcpy(profile.surename, "Demais");
    strcpy(profile.email, "aqua@gg.com");
    strcpy(profile.picture, "sharon.jpg");
    strcpy(profile.address, "SP");
    strcpy(profile.education, "Pediatria");
    strcpy(profile.skills, "Costurar, Terror, dançar, atuar");
    memset(profile.experiences, 0, 640);
    strcpy(profile.experiences[0], "RPDRP S10");
    add_profile(&profile);

    strcpy(profile.name, "Alaska");
    strcpy(profile.surename, "5000");
    strcpy(profile.email, "alaska@gg.com");
    strcpy(profile.picture, "alaska.jpg");
    strcpy(profile.address, "Campinas");
    strcpy(profile.education, "Ambiental");
    strcpy(profile.skills, "Costurar, vender, dançar, atuar");
    memset(profile.experiences, 0, 640);
    strcpy(profile.experiences[0], "RPDRP S5");
    strcpy(profile.experiences[1], "RPDRP AS2");
    add_profile(&profile);

    strcpy(profile.name, "Miss");
    strcpy(profile.surename, "Vanjie");
    strcpy(profile.email, "vanjie@gg.com");
    strcpy(profile.picture, "vanjie.jpeg");
    strcpy(profile.address, "Sorocaba");
    strcpy(profile.education, "Ambiental");
    strcpy(profile.skills, "Comédia, andar, costurar");
    memset(profile.experiences, 0, 640);
    strcpy(profile.experiences[0], "RPDRP S10");
    strcpy(profile.experiences[1], "RPDRP A11");
    add_profile(&profile);
    
    strcpy(profile.name, "Alexis");
    strcpy(profile.surename, "Mateo");
    strcpy(profile.email, "bam@gg.com");
    strcpy(profile.picture, "vanjie.jpeg");
    strcpy(profile.address, "Sorocaba");
    strcpy(profile.education, "Ambiental");
    strcpy(profile.skills, "KKKK, dançar, atuar");
    memset(profile.experiences, 0, 640);
    strcpy(profile.experiences[0], "RPDRP S3");
    add_profile(&profile);
    
    strcpy(profile.name, "Kim");
    strcpy(profile.surename, "K");
    strcpy(profile.email, "kimk@gg.com");
    strcpy(profile.picture, "vanjie.jpeg");
    strcpy(profile.address, "Hortolândia");
    strcpy(profile.education, "Ambiental");
    strcpy(profile.skills, "N/A");
    memset(profile.experiences, 0, 640);
    strcpy(profile.experiences[0], "Reality show");
    add_profile(&profile);
    
    strcpy(profile.name, "Siri");
    strcpy(profile.surename, "Apple");
    strcpy(profile.email, "siri@i.com");
    strcpy(profile.picture, "alaska.jpg");
    strcpy(profile.address, "SP");
    strcpy(profile.education, "Computação");
    strcpy(profile.skills, "Falar");
    memset(profile.experiences, 0, 640);
    strcpy(profile.experiences[0], "Assistente virtual na Apple");
    add_profile(&profile);
    
    strcpy(profile.name, "Cortana");
    strcpy(profile.surename, "MSFT");
    strcpy(profile.email, "cortana@ms.com");
    strcpy(profile.picture, "ariel.jpg");
    strcpy(profile.address, "Campinas");
    strcpy(profile.education, "Computação");
    strcpy(profile.skills, "Falar e computar");
    memset(profile.experiences, 0, 640);
    strcpy(profile.experiences[0], "Assistente virtual na Microsoft");
    add_profile(&profile);
    
    strcpy(profile.name, "Nina");
    strcpy(profile.surename, "Brown");
    strcpy(profile.email, "nina@gg.com");
    strcpy(profile.picture, "vanjie.jpeg");
    strcpy(profile.address, "Valinhos");
    strcpy(profile.education, "Moda");
    strcpy(profile.skills, "Dançar, desfilar");
    memset(profile.experiences, 0, 640);
    strcpy(profile.experiences[0], "RPDRP S9");
    add_profile(&profile);
    
    strcpy(profile.name, "Shea");
    strcpy(profile.surename, "Coulee");
    strcpy(profile.email, "win@gg.com");
    strcpy(profile.picture, "alaska.jpg");
    strcpy(profile.address, "SP");
    strcpy(profile.education, "Moda");
    strcpy(profile.skills, "Dançar, atuar, costurar, desfilar, ganhar");
    memset(profile.experiences, 0, 640);
    strcpy(profile.experiences[0], "RPDRP S9");
    strcpy(profile.experiences[0], "RPDRP AS5");
    add_profile(&profile);
    
    strcpy(profile.name, "Todrick");
    strcpy(profile.surename, "Hall");
    strcpy(profile.email, "th@gg.com");
    strcpy(profile.picture, "vanjie.jpeg");
    strcpy(profile.address, "SP");
    strcpy(profile.education, "Design");
    strcpy(profile.skills, "Dançar, cantar, atuar");
    memset(profile.experiences, 0, 640);
    strcpy(profile.experiences[0], "RPDRP - jurado especial");
    strcpy(profile.experiences[0], "Apresentador de TV");
    add_profile(&profile);
    
    strcpy(profile.name, "Raja");
    strcpy(profile.surename, "Sutan");
    strcpy(profile.email, "raja@gg.com");
    strcpy(profile.picture, "sharon.jpg");
    strcpy(profile.address, "Hortolândia");
    strcpy(profile.education, "Arquitetura");
    strcpy(profile.skills, "Dançar, atuar, costurar, desfilar");
    memset(profile.experiences, 0, 640);
    strcpy(profile.experiences[0], "RPDRP S3");
    strcpy(profile.experiences[0], "RPDRP FPR");
    add_profile(&profile);
    
    strcpy(profile.name, "AAAAAAA");
    strcpy(profile.surename, "SASASA");
    strcpy(profile.email, "aaa@gg.com");
    strcpy(profile.picture, "vanjie.jpeg");
    strcpy(profile.address, "Valinhos");
    strcpy(profile.education, "Ambiental");
    strcpy(profile.skills, "Skill 1, Skill 2");
    memset(profile.experiences, 0, 640);
    strcpy(profile.experiences[0], "Exp 1 AAAAAAAAAAAAA");
    add_profile(&profile);
    
    strcpy(profile.name, "BBBB");
    strcpy(profile.surename, "BABA");
    strcpy(profile.email, "ba@gg.com");
    strcpy(profile.picture, "alaska.jpg");
    strcpy(profile.address, "Sorocaba");
    strcpy(profile.education, "Arte");
    strcpy(profile.skills, "Skill 1, 2, 3");
    memset(profile.experiences, 0, 640);
    strcpy(profile.experiences[0], "Exp 1 - Grande experiencia");
    strcpy(profile.experiences[0], "Exp 2 - Pequena experiencia");
    add_profile(&profile);
}
